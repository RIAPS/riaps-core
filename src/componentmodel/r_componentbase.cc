//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_componentbase.h>
#include <utils/r_utils.h>


namespace riaps{

    void component_actor(zsock_t* pipe, void* args){
        ComponentBase* comp = (ComponentBase*)args;

        zsock_t* timerport = zsock_new_pull(comp->getTimerChannel().c_str());
        assert(timerport);


        zsock_t* timerportOneShot = zsock_new_pull(comp->getOneShotTimerChannel().c_str());
        assert(timerportOneShot);

        zpoller_t* poller = zpoller_new(pipe, NULL);
        assert(poller);

        // New api is czmq, ignore_interrupts is obsolote
        zpoller_set_nonstop(poller, true);
        zsock_signal (pipe, 0);

        int rc = zpoller_add(poller, timerport);
        assert(rc==0);

        rc = zpoller_add(poller, timerportOneShot);
        assert(rc==0);

        bool terminated = false;
        bool firstrun = true;

        auto consoleLogger = comp->_logger;
        consoleLogger->info("Component started");

        // Register ZMQ Socket - riapsPort pairs. For the quick retrieve.
        std::unordered_map<const zsock_t*, const ports::PortBase*>   portSockets;

        // ZMQ Socket - Inside port pairs
        //std::map<const zsock_t*, const ports::InsidePort*> insidePorts;

        // TODO: If leader election is enabled use dynamic timeout in the poller
        while (!terminated) {
            void *which = zpoller_wait(poller, 10);

            if (firstrun) {
                firstrun = false;

                const component_conf& comp_conf = comp->GetConfig();

                // Add insider ports
                for (auto it_insconf = comp_conf.component_ports.inss.begin();
                     it_insconf != comp_conf.component_ports.inss.end();
                     it_insconf++){
                    const ports::InsidePort* newPort = comp->initInsidePort(*it_insconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    //insidePorts[zmqSocket] = newPort;
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start timers
                for (auto it_timconf = comp_conf.component_ports.tims.begin();
                          it_timconf != comp_conf.component_ports.tims.end();
                          it_timconf++){
                    // Don't put the zmqSocket into portSockets[zmqSocket], just one timer port exist in the component.
                    // Cannot differentiate timerports based on ZMQ Sockets.
                    comp->initTimerPort(*it_timconf);
                }

                // Add and start publishers
                for (auto it_pubconf = comp_conf.component_ports.pubs.begin();
                          it_pubconf != comp_conf.component_ports.pubs.end();
                          it_pubconf++){
                    const ports::PublisherPort* newPort = comp->initPublisherPort(*it_pubconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();

                    portSockets[zmqSocket] = newPort;
                }

                // Add and start response ports
                for (auto it_repconf = comp_conf.component_ports.reps.begin();
                     it_repconf != comp_conf.component_ports.reps.end();
                     it_repconf++){

                    const ports::ResponsePort* newPort = comp->initResponsePort(*it_repconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start answer ports
                for (auto it_ansconf = comp_conf.component_ports.anss.begin();
                     it_ansconf != comp_conf.component_ports.anss.end();
                     it_ansconf++){

                    const ports::AnswerPort* newPort = comp->initAnswerPort(*it_ansconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add RequestPorts
                // Add the request port to the poller, just to be compatible with riaps-pycom
                // (and support the not really "async" behavior)
                for (auto it_reqconf = comp_conf.component_ports.reqs.begin();
                          it_reqconf!= comp_conf.component_ports.reqs.end();
                          it_reqconf++) {

                    const ports::RequestPort* newPort = comp->initRequestPort(*it_reqconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add query ports
                for (auto it_qryconf = comp_conf.component_ports.qrys.begin();
                     it_qryconf!= comp_conf.component_ports.qrys.end();
                     it_qryconf++) {

                    const ports::QueryPort* newPort = comp->initQueryPort(*it_qryconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start subscribers
                for (auto it_subconf = comp_conf.component_ports.subs.begin();
                          it_subconf != comp_conf.component_ports.subs.end();
                          it_subconf++) {
                    const ports::SubscriberPort* newPort = comp->initSubscriberPort(*it_subconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }
            }

            if (which == pipe) {
                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    consoleLogger->warn("No msg => interrupted");
                    break;
                }

                char *command = zmsg_popstr(msg);

                if (streq(command, "$TERM")) {
                    consoleLogger->debug("$TERM arrived in component");
                    terminated = true;
                } else if(streq(command, CMD_UPDATE_PORT)){
                    char* portname = zmsg_popstr(msg);
                    if (portname){
                        char* host = zmsg_popstr(msg);
                        if (host){
                            char* port = zmsg_popstr(msg);
                            if (port){
                                auto port_tobe_updated = comp->GetPortByName(portname);

                                if (port_tobe_updated!=NULL){
                                    if (port_tobe_updated->GetPortType() == ports::PortTypes::Subscriber){
                                        // note: we assume that the publisher uses tcp://
                                        std::string new_pub_endpoint = "tcp://" + std::string(host) + ":" + std::string(port);
                                        ((ports::SubscriberPort*)port_tobe_updated)->ConnectToPublihser(new_pub_endpoint);
                                    } else if (port_tobe_updated->GetPortType() == ports::PortTypes::Request){
                                        std::string new_rep_endpoint = "tcp://" + std::string(host) + ":" + std::string(port);
                                        ((ports::RequestPort*)port_tobe_updated)->ConnectToResponse(new_rep_endpoint);
                                    } else if (port_tobe_updated->GetPortType() == ports::PortTypes::Query){
                                        std::string new_ans_endpoint = "tcp://" + std::string(host) + ":" + std::string(port);
                                        ((ports::QueryPort*)port_tobe_updated)->ConnectToResponse(new_ans_endpoint);
                                    }
                                }

                                zstr_free(&port);
                            }
                            zstr_free(&host);
                        }
                        zstr_free(&portname);
                    }
                }
                else if(streq(command, CMD_UPDATE_GROUP)){
                    zframe_t* capnp_msgbody = zmsg_pop(msg);
                    size_t    size = zframe_size(capnp_msgbody);
                    byte*     data = zframe_data(capnp_msgbody);

                    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

                    capnp::FlatArrayMessageReader reader(capnp_data);
                    auto msgDiscoUpd  = reader.getRoot<riaps::discovery::DiscoUpd>();
                    auto msgGroupUpd  = msgDiscoUpd.getGroupUpdate();
                    comp->UpdateGroup(msgGroupUpd);

                    zframe_destroy(&capnp_msgbody);
                }

                zstr_free(&command);
                zmsg_destroy(&msg);
            }
            else if (which == timerport) {
                zmsg_t *msg = zmsg_recv(which);

                if (msg) {
                    char *portName = zmsg_popstr(msg);
                    ports::PeriodicTimer* timerPort = (ports::PeriodicTimer*)comp->GetPortByName(portName);
                    std::vector<std::string> fields;

                    if (!terminated)
                        comp->DispatchMessage(NULL, timerPort);



                    zstr_free(&portName);
                    zmsg_destroy(&msg);
                }
            }
            // Message from one shot timer
            else if ((which == timerportOneShot) && !terminated){

                zmsg_t* msg = zmsg_recv(which);
                zframe_t* idframe = zmsg_pop(msg);
                uint64_t* timerId = reinterpret_cast<uint64_t*>(zframe_data(idframe));

                comp->m_scheduledAction(*timerId);
//                auto action = comp->m_scheduledActions.find(*timerId);
//                if (action == comp->m_scheduledActions.end())
//                    comp->OnScheduledTimer(*timerId);
//                else
//                    action->second();

                zmsg_destroy(&msg);
                zframe_destroy(&idframe);




            }
            else if(which){

                    ports::PortBase *riapsPort = const_cast<ports::PortBase *>(portSockets[static_cast<zsock_t *>(which)]);

                    // If the port is async, the frames are different
                    if (riapsPort->AsAnswerPort() != nullptr || riapsPort->AsQueryPort() != nullptr) {
                        char     *originId, *messageId;
                        zframe_t *body, *timestamp;

                        std::shared_ptr <riaps::MessageParams> params = nullptr;
                        capnp::FlatArrayMessageReader *capnpReader = nullptr;

                        /**
                         * The query port doesn't have SocketId in the first frame, while answer port has it,
                         * thus different pattern is used for recv()
                         *  If the port is answerport: |String  |String   |Frame  |Frame    |
                         *                             |SocketId|RequestId|Message|Timestamp|
                         *
                         *  If the port is queryport:  |String   |Frame  |Frame    |
                         *                             |RequestId|Message|Timestamp|
                         */
                        if (riapsPort->AsAnswerPort()) {

                            if (zsock_recv(which, "ssff", &originId, &messageId, &body, &timestamp) == 0) {
                                // Takes the ownership, deletes originId, messageId and timestamp
                                params = std::shared_ptr<riaps::MessageParams>(new MessageParams(&originId, &messageId, &timestamp));
                            } else {
                                comp->_logger->error("AnswerPort ({}) frames are incorrect.", riapsPort->GetPortName());
                            }
                        } else if (riapsPort->AsQueryPort()){
                            if (zsock_recv(which, "sff", &messageId, &body, &timestamp) == 0) {
                                params = std::shared_ptr<riaps::MessageParams>(new MessageParams(&messageId, &timestamp));
                            } else {
                                comp->_logger->error("QueryPort ({}) frames are incorrect.", riapsPort->GetPortName());
                            }
                        }

                        /**
                         * Convert to capnp buffer only if the recv() was successful
                         */
                        if (body!=nullptr && !terminated) {
                            (*body) >> capnpReader;
                            comp->DispatchMessage(capnpReader, riapsPort, params);
                        }

                        //if (capnpReader != nullptr)
                        //    delete capnpReader;

                        zframe_destroy(&body);
                    } else if(riapsPort->AsInsidePort() != nullptr){
                        zmsg_t* msg = zmsg_recv(which);
                        auto insidePort = riapsPort->AsInsidePort();
                        if (!terminated && msg) {
                            comp->DispatchInsideMessage(msg, insidePort);

                        }
                        if (msg){
                            zmsg_destroy(&msg);
                        }
                    } else {

                        if (riapsPort->AsRecvPort()!= nullptr) {
                            //Note: new recv() implementation with timestamp processing
                            auto recvPort = riapsPort->AsRecvPort();

                            auto cname = comp->GetComponentName();
                            auto portName = riapsPort->GetPortName();

                            auto capnpMessage = recvPort->Recv();
                            if (!terminated)
                                comp->DispatchMessage(capnpMessage.get(), riapsPort);
                        } else {

                            // Note: this is the old style, timestamps are not read
                            zmsg_t* msg = zmsg_recv(which);
                            zframe_t* bodyFrame = zmsg_pop(msg);

                            auto msgPtr = new std::shared_ptr<zmsg_t>(msg, [](zmsg_t* z){zmsg_destroy(&z);});
                            auto framePtr = new std::shared_ptr<zframe_t>(bodyFrame, [](zframe_t* z){zframe_destroy(&z);});

                            // zmsg_pop transfers the ownership, the frame is removed from the zmsg.
                            // Both of them must be explicitly deleted.
                            auto capnpReader = std::unique_ptr<capnp::FlatArrayMessageReader>(new capnp::FlatArrayMessageReader(nullptr));

                            (*bodyFrame) >> capnpReader;

                            if (!terminated)
                                comp->DispatchMessage(capnpReader.get(), riapsPort);

                            zframe_destroy(&bodyFrame);
                            zmsg_destroy(&msg);
                        }


                    }
            }
            else{
                // just poll timeout
            }

            // The group messages
            if (comp->m_groups.size()>0){

                // Handle all group messages
                for (auto it = comp->m_groups.begin(); it!=comp->m_groups.end(); it++){
                    it->second->SendPingWithPeriod();

                    //std::shared_ptr<capnp::FlatArrayMessageReader> groupMessage(nullptr);

                    //std::string originComponentId;
                    //ports::GroupSubscriberPort* groupRecvPort = it->second->FetchNextMessage(groupMessage);
                    it->second->FetchNextMessage();

                    //if (groupRecvPort != nullptr)
                        //comp->OnGroupMessage(it->first, *groupMessage, groupRecvPort);
                }
            }
        }

//        if (comp->_oneShotTimer!=NULL){
//            comp->_oneShotTimer->stop();
//            delete comp->_oneShotTimer;
//            comp->_oneShotTimer = NULL;
//        }

        zpoller_destroy(&poller);
        zsock_destroy(&timerportOneShot);
        zsock_destroy(&timerport);
    };

    void ComponentBase::StopComponent() {
        //_execute.store(false);
        // Stop timers
        for (auto& config : GetConfig().component_ports.tims){
            std::cout << "Stop timer: " << config.portName << std::endl;
            m_ports[config.portName]->AsTimerPort()->stop();
        }

        zactor_destroy(&m_zactorComponent);

        // Delete ports
        for (auto it = m_ports.begin(); it!=m_ports.end(); it++){
            auto port = it->second.release();
            delete port;
        }

        zuuid_destroy(&m_componentUuid);
    }

    void ComponentBase::handleCPULimit() {
        _logger->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "CPU limit"
                , __func__
                , GetActor()->getApplicationName()
                , GetActor()->getActorName()
                , GetComponentName());
    }

    void ComponentBase::handleMemLimit() {
        _logger->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Memory limit"
                , __func__
                , GetActor()->getApplicationName()
                , GetActor()->getActorName()
                , GetComponentName());
    }

    void ComponentBase::handleNetLimit() {
        _logger->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Net limit"
                , __func__
                , GetActor()->getApplicationName()
                , GetActor()->getActorName()
                , GetComponentName());
    }

    void ComponentBase::handleSpcLimit() {
        _logger->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Space limit"
                , __func__
                , GetActor()->getApplicationName()
                , GetActor()->getActorName()
                , GetComponentName());

    }

//    std::shared_ptr<spd::logger> ComponentBase::GetConsoleLogger(){
//        return m_logger;
//    }

    void ComponentBase::SetDebugLevel(std::shared_ptr<spd::logger> logger, spd::level::level_enum level){
        logger->set_level(level);
    }

    void ComponentBase::OnScheduledTimer(uint64_t timerId) {
        _logger->error("Scheduled timer is fired, but no handler is implemented. Implement OnSchedulerTimer() in component {}", GetConfig().component_name);
    }

    ComponentBase::ComponentBase(component_conf& config, Actor& actor)
            : m_actor(&actor),
              m_timerCounter(0)
    {
        m_configuration = config;

        //uuid to the component instance
        m_componentUuid = zuuid_new();

        size_t q_size = 2048; //queue size must be power of 2
        spd::set_async_mode(q_size);

        _logger = spd::get(m_configuration.component_name);
        if (_logger == nullptr)
            _logger = spd::stdout_color_mt(m_configuration.component_name);

        m_zactorComponent = zactor_new(component_actor, this);
    }

    // For the timer port
//    void ComponentBase::DispatchMessage(const std::string &messagetype, msgpack::sbuffer* message,
//                                        ports::PortBase *port) {
//        auto handler = GetHandler(port->GetPortName());
//        (this->*handler)(messagetype, message, port);
//    }



//    const ports::CallBackTimer* ComponentBase::InitTimerPort(const _component_port_tim_j& config) {
//        std::string timerchannel = getTimerChannel();
//        std::unique_ptr<ports::CallBackTimer> newtimer(new ports::CallBackTimer(timerchannel, config));
//        newtimer->start(config.period);
//
//        auto result = newtimer.get();
//
//        _ports[config.portName] = std::move(newtimer);
//        return result;
//    }



    ports::PublisherPort* ComponentBase::GetPublisherPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == NULL) return NULL;
        return portBase->AsPublishPort();
    }

    ports::QueryPort* ComponentBase::GetQueryPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == NULL) return NULL;
        return portBase->AsQueryPort();
    }

    ports::RequestPort* ComponentBase::GetRequestPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == NULL) return NULL;
        return portBase->AsRequestPort();
    }

    ports::ResponsePort* ComponentBase::GetResponsePortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == NULL) return NULL;
        return portBase->AsResponsePort();
    }

    void ComponentBase::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                       capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {
        _logger->error("Group message arrived, but no handler implemented in the component");
    }

    void ComponentBase::OnMessageToLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message) {
        _logger->error("Group message arrived to the leader, but {} is not implemented in component: {}",
                       __FUNCTION__,
                       GetComponentName());
    }

    /// \param portName
    /// \return Pointer to the RIAPS port with the given name. NULL if the port was not found.
    ports::PortBase* ComponentBase::GetPortByName(const std::string & portName) {
        if (m_ports.find(portName)!=m_ports.end()){
            return m_ports[portName].get();
        }

        return NULL;
    }

    const component_conf& ComponentBase::GetConfig() const {
        return m_configuration;
    }

    const Actor* ComponentBase::GetActor() const{
        return m_actor;
    }



    const ports::PublisherPort* ComponentBase::initPublisherPort(const component_port_pub& config) {
        auto result = new ports::PublisherPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    bool ComponentBase::SendMessageOnPort(zmsg_t** message, const std::string& portName) {
        auto port = GetPortByName(portName);
        if (port == NULL) return false;

        auto insidePort = port->AsInsidePort();

        return insidePort->Send(message);
    }

    bool ComponentBase::SendLeaderMessage(const riaps::groups::GroupId &groupId,
                                          capnp::MallocMessageBuilder &message) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return false;
        if (!IsLeader(group)) return false;
        return group->SendLeaderMessage(message);
    }



    const ports::SubscriberPort* ComponentBase::initSubscriberPort(const component_port_sub& config) {
        std::unique_ptr<ports::SubscriberPort> newport(new ports::SubscriberPort(config, this));
        auto result = newport.get();
        newport->Init();
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::ResponsePort* ComponentBase::initResponsePort(const component_port_rep & config) {
        auto result = new ports::ResponsePort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::RequestPort*   ComponentBase::initRequestPort(const component_port_req& config){
        std::unique_ptr<ports::RequestPort> newport(new ports::RequestPort(config, this));
        auto result = newport.get();
        newport->Init();
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::AnswerPort* ComponentBase::initAnswerPort(const component_port_ans & config) {
        auto result = new ports::AnswerPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::QueryPort* ComponentBase::initQueryPort(const component_port_qry & config) {
        std::unique_ptr<ports::QueryPort> newport(new ports::QueryPort(config, this));
        auto result = newport.get();
        newport->Init();
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::InsidePort* ComponentBase::initInsidePort(const component_port_ins& config) {
        auto result = new ports::InsidePort(config, riaps::ports::InsidePortMode::BIND, this);
        std::unique_ptr<ports::PortBase> newport(result);
        m_ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::PeriodicTimer* ComponentBase::initTimerPort(const component_port_tim& config) {
        std::string timerchannel = getTimerChannel();
        std::unique_ptr<ports::PeriodicTimer> newtimer(new ports::PeriodicTimer(timerchannel, config, this));
        newtimer->start();

        auto result = newtimer.get();

        m_ports[config.portName] = std::move(newtimer);
        return result;
    }

//    bool ComponentBase::SendMessageOnPort(zmsg_t **message, const std::string &portName) {
//        ports::PortBase* port = GetPortByName(portName);
//        if (port == NULL) return false;
//
//        ports::SenderPort* senderPort = dynamic_cast<ports::SenderPort*>(port);
//        if (senderPort == nullptr) return false;
//        senderPort->Send()
//
////        if (port->AsSubscribePort() == NULL &&
////            port->AsTimerPort() == NULL     &&
////            port->As){
////            return port->Send(message);
////        }
////
////        return false;
//    }

//    bool ComponentBase::SendMessageOnPort(msgpack::sbuffer &message, const std::string &portName) {
//        zmsg_t* msg = zmsg_new();
//        zmsg_pushmem(msg, message.data(), message.size());
//        return SendMessageOnPort(&msg, portName);
//
//    }
//
//    bool ComponentBase::SendMessageOnPort(MessageBase* message, const std::string &portName) {
//        zmsg_t* msg = message->AsZmqMessage();
//        return SendMessageOnPort(&msg, portName);
//    }
//
    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder& message,
                                          const std::string &portName) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == NULL) return false;

        ports::SenderPort* senderPort = dynamic_cast<ports::SenderPort*>(port);
        if (senderPort == nullptr) return false;
        return senderPort->Send(message);
    }

    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder &message, const std::string &portName,
                                          std::shared_ptr<riaps::MessageParams> params) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == NULL) return false;

        ports::AnswerPort* answerPort = dynamic_cast<ports::AnswerPort*>(port);
        if (answerPort == nullptr) return false;
        return answerPort->SendAnswer(message, params);
    }

    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder &message, const std::string &portName,
                                          std::string &requestId) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == NULL) return false;

        ports::QueryPort* queryPort = port->AsQueryPort();
        if (queryPort == nullptr) return false;
        return queryPort->SendQuery(message, requestId);
    }

    bool ComponentBase::SendGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::MallocMessageBuilder &message,
                                         const std::string& portName) {
        // Search the group
        if (m_groups.find(groupId)==m_groups.end()) return false;

        riaps::groups::Group* group = m_groups[groupId].get();

        return group->SendMessage(message, portName);

    }

    bool ComponentBase::SendGroupMessage(const riaps::groups::GroupId&& groupId,
                                         capnp::MallocMessageBuilder &message,
                                         const std::string& portName) {
        return SendGroupMessage(groupId, message, portName);

    }

    bool ComponentBase::SendMessageToLeader(const riaps::groups::GroupId &groupId,
                                            capnp::MallocMessageBuilder &message) {
        auto group = getGroupById(groupId);
        if (group == nullptr){
            return false;
        }

        return group->SendMessageToLeader(message);
    }

    void ComponentBase::OnMessageFromLeader(const riaps::groups::GroupId &groupId,
                                            capnp::FlatArrayMessageReader &message) {
        _logger->debug("Message from the leader arrived, but no OnMessageFromHandler() implementation has found in component: {}", GetConfig().component_name);
    }

    riaps::groups::Group* ComponentBase::getGroupById(const riaps::groups::GroupId &groupId) {
        if (m_groups.find(groupId)==m_groups.end()) return nullptr;

        return m_groups[groupId].get();
    }


//    const ports::PortBase* ComponentBase::GetPort(std::string portName) const {
//        auto port_it = _ports.find(portName);
//        if (port_it!=_ports.end()){
//            return port_it->second.get();
//        }
//        return nullptr;
//    }


    std::string ComponentBase::getTimerChannel() {
        std::string prefix= "inproc://timer";
        return prefix + GetCompUuid();
    }

    std::string ComponentBase::getOneShotTimerChannel() {
        std::string prefix= "inproc://oneshottimer";
        return prefix + GetCompUuid();
    }

    const std::string ComponentBase::GetCompUuid() const{
        std::string uuid_str = zuuid_str(m_componentUuid);
        return uuid_str;
    }

    zactor_t* ComponentBase::GetZmqPipe() const {
        return m_zactorComponent;
    }

    void ComponentBase::PrintMessageOnPort(ports::PortBase *port, std::string message) {

        if (port == NULL) return;
        std::string direction = (port->AsSubscribePort()!=NULL || port->AsResponsePort()!=NULL || port->AsTimerPort()!=NULL) ?
                                "=> " : "<= ";

        std::cout << direction
                  << GetConfig().component_type
                  << "::"
                  << port->GetPortBaseConfig()->portName
                  << ((port->GetPortBaseConfig()->messageType=="")?"":" -> " + port->GetPortBaseConfig()->messageType)
                  << ((message=="")?"":" -> " + message)
                  << std::endl;
    }

    void ComponentBase::PrintParameters() {
        auto parameters = m_configuration.component_parameters.getParameterNames();
        for (auto it = parameters.begin(); it!=parameters.end(); it++){
            std::cout << *it << " : " << m_configuration.component_parameters.getParam(*it)->getValueAsString() << std::endl;
        }
    }

    // TODO: Remove timeout parameter
    uint16_t ComponentBase::GetGroupMemberCount(const riaps::groups::GroupId &groupId, int64_t timeout) {
        if (m_groups.find(groupId)==m_groups.end())
            return 0;

        return m_groups[groupId]->GetMemberCount();
    }

    std::string ComponentBase::GetLeaderId(const riaps::groups::GroupId &groupId) {
        if (m_groups.find(groupId)==m_groups.end())
            return "";

        return m_groups[groupId]->GetLeaderId();
    }

    bool ComponentBase::JoinGroup(riaps::groups::GroupId &&groupId) {
        return JoinGroup(groupId);
    }

    bool ComponentBase::JoinGroup(riaps::groups::GroupId &groupId) {
        if (m_groups.find(groupId)!=m_groups.end())
            return false;

        auto newGroupPtr = new riaps::groups::Group(groupId, this);
        auto newGroup = std::unique_ptr<riaps::groups::Group>(
                newGroupPtr
        );

        if (newGroup->InitGroup()) {
            m_groups[groupId] = std::move(newGroup);
            return true;
        }

        return false;
    }

    bool ComponentBase::LeaveGroup(riaps::groups::GroupId &&groupId) {
        return JoinGroup(groupId);
    }

    bool ComponentBase::LeaveGroup(riaps::groups::GroupId &groupId) {
        if (m_groups.find(groupId) == m_groups.end())
            return false;
        m_groups.erase(groupId);
        return true;
    }

    std::vector<riaps::groups::GroupId> ComponentBase::GetGroupMemberships() {
        std::vector<riaps::groups::GroupId> results;

        for(auto& group : m_groups) {
            results.push_back(group.first);
        }

        return results;
    }

    bool ComponentBase::IsMemberOf(riaps::groups::GroupId &groupId) {
        for(auto& group : m_groups) {
            if (group.first == groupId)
                return true;
        }
        return false;
    }

    bool ComponentBase::IsLeader(const riaps::groups::Group* group) {
        return GetCompUuid() == group->GetLeaderId();
    }

    bool ComponentBase::IsLeader(const riaps::groups::GroupId &groupId) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return false;
        return IsLeader(group);
    }

    bool ComponentBase::IsLeaderAvailable(const riaps::groups::GroupId &groupId) {
        if (m_groups.find(groupId) == m_groups.end())
            return false;
        return m_groups[groupId]->GetLeaderId()!="";
    }

    std::vector<riaps::groups::GroupId> ComponentBase::GetGroupMembershipsByType(const std::string &groupType) {
        std::vector<riaps::groups::GroupId> results;
        for(auto& group : m_groups) {
            if (group.first.groupTypeId != groupType) continue;
            results.push_back(group.first);
        }
        return results;
    }

//    uint64_t ComponentBase::ScheduleTimer(std::chrono::steady_clock::time_point &tp) {
//        std::string timerChannel = getOneShotTimerChannel();
//        uint64_t timerId = _timerCounter;
//        std::thread t([tp, timerChannel, timerId](){
//            auto ptr  = std::shared_ptr<zsock_t>(zsock_new_push(timerChannel.c_str()),[](zsock_t* z){
//                zsock_destroy(&z);
//                zclock_sleep(500);
//            });
//
//            uint64_t targetTime = duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
//            auto earlyTp = tp-duration<uint64_t, std::micro>(500);
//            std::this_thread::sleep_until(earlyTp);
//            zsock_send(ptr.get(),"88", timerId, targetTime);
//            zclock_sleep(500);
//        });
//        t.detach();
//        return _timerCounter++;
//    }

    uint64_t ComponentBase::ScheduleAbsTimer(const timespec &tp, uint64_t wakeupOffset) {
        std::string timerChannel = getOneShotTimerChannel();
        uint64_t timerId = m_timerCounter;

        std::thread t([tp, timerChannel, timerId, wakeupOffset](){
            auto ptr  = std::shared_ptr<zsock_t>(zsock_new_push(timerChannel.c_str()),[](zsock_t* z){
                zsock_destroy(&z);
                zclock_sleep(500);
            });

            // Wake up earlier by 500 microsec
//            timespec earlyWakeOffset;
//            earlyWakeOffset.tv_sec  = wakeupOffset/BILLION;
//            earlyWakeOffset.tv_nsec = wakeupOffset%BILLION;
//

            // Calculate the new wakeup time
            timespec earlyWakeup;
            earlyWakeup.tv_sec  = tp.tv_sec  - (wakeupOffset/BILLION);
            earlyWakeup.tv_nsec = tp.tv_nsec - (wakeupOffset%BILLION);
            if (earlyWakeup.tv_nsec<0){
                earlyWakeup.tv_sec--;
                earlyWakeup.tv_nsec+=BILLION;
            }

            zmsg_t* msg = zmsg_new();

            zmsg_addmem(msg, &timerId, sizeof(decltype(timerId)));



            clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &earlyWakeup, NULL);
            zmsg_send(&msg, ptr.get());
            zclock_sleep(400);
        });
        t.detach();
        return m_timerCounter++;
    }

    uint64_t ComponentBase::ScheduleAction(const timespec &tp,
                                           std::function<void(const uint64_t)> action,
                                           uint64_t wakeupOffset) {
        std::string timerChannel = getOneShotTimerChannel();
        uint64_t timerId = m_timerCounter;
        m_scheduledAction = action;

        std::thread t([tp, timerChannel, timerId, wakeupOffset](){
            auto ptr  = std::shared_ptr<zsock_t>(zsock_new_push(timerChannel.c_str()),[](zsock_t* z){
                zsock_destroy(&z);
                zclock_sleep(500);
            });

            // Wake up earlier by 500 microsec
//            timespec earlyWakeOffset;
//            earlyWakeOffset.tv_sec  = wakeupOffset/BILLION;
//            earlyWakeOffset.tv_nsec = wakeupOffset%BILLION;
//

            // Calculate the new wakeup time
            timespec earlyWakeup;
            earlyWakeup.tv_sec  = tp.tv_sec  - (wakeupOffset/BILLION);
            earlyWakeup.tv_nsec = tp.tv_nsec - (wakeupOffset%BILLION);
            if (earlyWakeup.tv_nsec<0){
                earlyWakeup.tv_sec--;
                earlyWakeup.tv_nsec+=BILLION;
            }

            zmsg_t* msg = zmsg_new();

            zmsg_addmem(msg, &timerId, sizeof(decltype(timerId)));



            clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &earlyWakeup, NULL);
            zmsg_send(&msg, ptr.get());
            zclock_sleep(400);
        });
        t.detach();
        return m_timerCounter++;
    }

    //TODO: only if the difference NOT in -50/+50 microsec
    timespec ComponentBase::WaitUntil(const timespec &targetTimepoint) {
        timespec earlyWakeup = targetTimepoint;
        earlyWakeup.tv_nsec-=TIMER_ACCURACY;

        if (earlyWakeup.tv_nsec<0){
            earlyWakeup.tv_sec--;
            earlyWakeup.tv_nsec+=BILLION;
        }

        while (true){
            timespec now;
            clock_gettime(CLOCK_REALTIME, &now);

            if (now>earlyWakeup){
                return now;
            }
        }
    }



    void ComponentBase::OnAnnounce(const riaps::groups::GroupId &groupId, const std::string &proposeId, bool accepted) {
        _logger->error("Vote result is announced, but no handler implemented in component {}", GetComponentName());
    }

    void ComponentBase::OnPropose(riaps::groups::GroupId &groupId, const std::string &proposeId,
                                  capnp::FlatArrayMessageReader &message) {
        _logger->error("Leader proposed a value but no handler is implemented in component {}", GetComponentName());
    }

    void ComponentBase::OnActionPropose(riaps::groups::GroupId &groupId,
                                        const std::string      &proposeId,
                                        const std::string      &actionId,
                                        const timespec         &timePoint) {
        _logger->info("Leader proposed an action, but no handler is implemented in component {}", GetComponentName());
    }

    const std::string ComponentBase::GetComponentName() const {
        return GetConfig().component_name;
    }

    std::string ComponentBase::SendPropose(const riaps::groups::GroupId &groupId, capnp::MallocMessageBuilder &message) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return "";

        zuuid_t* uuid = zuuid_new();
        std::string strUuid = zuuid_str(uuid);
        zuuid_destroy(&uuid);

        if (group->ProposeValueToLeader(message, strUuid)){
            return strUuid;
        }
        return "";
    }

    std::string ComponentBase::ProposeAction(const riaps::groups::GroupId &groupId,
                                             const std::string &actionId,
                                             const timespec &absTime) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return "";

        zuuid_t* uuid = zuuid_new();
        std::string strUuid = zuuid_str(uuid);
        zuuid_destroy(&uuid);

        if (group->ProposeActionToLeader(strUuid, actionId, absTime)){
            return strUuid;
        }
        return "";
    }

    bool ComponentBase::SendVote(const riaps::groups::GroupId &groupId, const std::string &proposeId, bool accept) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return false;

        return group->SendVote(proposeId, accept);
    }

    void ComponentBase::UpdateGroup(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
        // First, find the affected groups
        riaps::groups::GroupId gid;
        gid.groupName   = msgGroupUpdate.getGroupId().getGroupName().cStr();
        gid.groupTypeId = msgGroupUpdate.getGroupId().getGroupType().cStr();

        if (m_groups.find(gid) == m_groups.end()) return;

        m_groups[gid]->ConnectToNewServices(msgGroupUpdate);
    }

    ComponentBase::~ComponentBase() {

    }

}