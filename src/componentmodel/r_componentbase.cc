#include <componentmodel/r_componentbase.h>
#include <utils/r_utils.h>
#include <functional>

using namespace std;

namespace riaps{

    void component_actor(zsock_t* pipe, void* args){

        auto comp = (ComponentBase*)args;

        zsock_t* pycontrol = zsock_new_pair(fmt::format("inproc://part_{}_control", comp->component_config().component_name).c_str());
        assert(pycontrol);

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

        rc = zpoller_add(poller, pycontrol);
        assert(rc==0);

        bool terminated = false;
        bool firstrun = true;

        auto compbase_logger = comp->riaps_logger_;
        compbase_logger->info("Component started");

        // Register ZMQ Socket - riapsPort pairs. For the quick retrieve.
        std::unordered_map<const zsock_t*, const ports::PortBase*>   portSockets;

        // ZMQ Socket - Inside port pairs
        //std::map<const zsock_t*, const ports::InsidePort*> insidePorts;

        // TODO: If leader election is enabled use dynamic timeout in the poller
        while (!terminated) {
            void *which = zpoller_wait(poller, 10);
            if (firstrun) {
                firstrun = false;

                const component_conf& comp_conf = comp->component_config();

                // Add insider ports
                for (auto& insconf : comp_conf.component_ports.inss) {
                    const ports::InsidePort* newPort = comp->InitInsidePort(insconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start timers
                for (auto& timconf : comp_conf.component_ports.tims) {
                    compbase_logger->debug("Register timer: {}", timconf.portName);
                    // Don't put the zmqSocket into portSockets[zmqSocket], just one timer port exist in the component.
                    // Cannot differentiate timerports based on ZMQ Sockets.
                    comp->InitTimerPort(timconf);
                }

                // Add and start publishers
                for (auto& pubconf : comp_conf.component_ports.pubs){
                    compbase_logger->debug("Register pub: {}", pubconf.portName);
                    const ports::PublisherPort* newPort = comp->InitPublisherPort(pubconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                }

                // Add and start response ports
                for (auto& repconf : comp_conf.component_ports.reps) {
                    compbase_logger->debug("Register REP: {}", repconf.portName);
                    const ports::ResponsePort* newPort = comp->InitResponsePort(repconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start answer ports
                for (auto& ansconf : comp_conf.component_ports.anss) {
                    const ports::AnswerPort* newPort = comp->InitAnswerPort(ansconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add RequestPorts
                // Add the request port to the poller, just to be compatible with riaps-pycom
                // (and support the not really "async" behavior)
                for (auto& reqconf : comp_conf.component_ports.reqs) {
                    const ports::RequestPort* newPort = comp->InitRequestPort(reqconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add query ports
                for (auto& qryconf : comp_conf.component_ports.qrys) {
                    const ports::QueryPort* newPort = comp->InitQueryPort(qryconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start subscribers
                for (auto& subconf : comp_conf.component_ports.subs) {
                    const ports::SubscriberPort* newPort = comp->InitSubscriberPort(subconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }
            }

            if (which == pipe) {
                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    compbase_logger->warn("No msg => interrupted");
                    break;
                }

                char *command = zmsg_popstr(msg);

                if (streq(command, "$TERM")) {
                    compbase_logger->debug("$TERM arrived");
                    terminated = true;
                } else if(streq(command, CMD_UPDATE_PORT)){
                    char* portname = zmsg_popstr(msg);
                    if (portname){
                        char* host = zmsg_popstr(msg);
                        if (host){
                            char* port = zmsg_popstr(msg);
                            if (port){
                                auto port_tobe_updated = comp->GetPortByName(portname);

                                if (port_tobe_updated != nullptr) {
                                    if (port_tobe_updated->GetPortType() == ports::PortTypes::Subscriber){
                                        // note: we assume that the publisher uses tcp://
                                        const string new_pub_endpoint = fmt::format("tcp://{}:{}", host, port);
                                        ((ports::SubscriberPort*)port_tobe_updated)->ConnectToPublihser(new_pub_endpoint);
                                    } else if (port_tobe_updated->GetPortType() == ports::PortTypes::Request){
                                        const string new_rep_endpoint = fmt::format("tcp://{}:{}", host, port);
                                        ((ports::RequestPort*)port_tobe_updated)->ConnectToResponse(new_rep_endpoint);
                                    } else if (port_tobe_updated->GetPortType() == ports::PortTypes::Query){
                                        const string new_ans_endpoint = fmt::format("tcp://{}:{}", host, port);
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
                    vector<string> fields;

                    if (!terminated) {
                        comp->DispatchMessage(nullptr, timerPort);
                    }
                    zstr_free(&portName);
                    zmsg_destroy(&msg);
                }
            }
            // Message from one shot timer
            else if ((which == timerportOneShot) && !terminated){

                zmsg_t* msg = zmsg_recv(which);
                zframe_t* idframe = zmsg_pop(msg);
                auto timerId = reinterpret_cast<uint64_t*>(zframe_data(idframe));

                comp->scheduled_action_(*timerId);
//                auto action = comp->m_scheduledActions.find(*timerId);
//                if (action == comp->m_scheduledActions.end())
//                    comp->OnScheduledTimer(*timerId);
//                else
//                    action->second();

                zmsg_destroy(&msg);
                zframe_destroy(&idframe);
            }
            else if (which == pycontrol) {
                compbase_logger->info("PYCONTORLL");
            }
            else if(which){
                compbase_logger->debug("Message on other ports");
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
                                params = make_shared<riaps::MessageParams>(&originId, &messageId, &timestamp);
                            } else {
                                compbase_logger->error("AnswerPort ({}) frames are incorrect.", riapsPort->GetPortName());
                            }
                        } else if (riapsPort->AsQueryPort()){
                            if (zsock_recv(which, "sff", &messageId, &body, &timestamp) == 0) {
                                params = make_shared<riaps::MessageParams>(&messageId, &timestamp);
                            } else {
                                compbase_logger->error("QueryPort ({}) frames are incorrect.", riapsPort->GetPortName());
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

                            auto cname = comp->component_name();
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
                            auto capnpReader = make_unique<capnp::FlatArrayMessageReader>(nullptr);

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
            if (!comp->groups_.empty()){

                // Handle all group messages
                for (auto& grp : comp->groups_){
                    grp.second->SendPingWithPeriod();

                    //std::shared_ptr<capnp::FlatArrayMessageReader> groupMessage(nullptr);

                    //std::string originComponentId;
                    //ports::GroupSubscriberPort* groupRecvPort = it->second->FetchNextMessage(groupMessage);
                    grp.second->FetchNextMessage();

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

    shared_ptr<spd::logger> ComponentBase::component_logger() {
        return component_logger_;
    }

    void ComponentBase::Setup() {

    }

    void ComponentBase::Activate() {
        component_zactor_ = zactor_new(component_actor, this);
    }

    void ComponentBase::Terminate() {
        // Stop timers
        for (auto& c : component_config().component_ports.tims){
            riaps_logger_->info("Stop timer: {}", c.portName);
            ports_[c.portName]->AsTimerPort()->stop();
        }

        zactor_destroy(&component_zactor_);

        // Delete ports
        for (auto& port : ports_){
            auto released_port = port.second.release();
            delete released_port;
        }

        zuuid_destroy(&component_uuid_);
    }

    void ComponentBase::HandleCPULimit() {
        riaps_logger_->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "CPU limit"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());
    }

    void ComponentBase::HandleMemLimit() {
        riaps_logger_->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Memory limit"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());
    }

    void ComponentBase::HandleNetLimit() {
        riaps_logger_->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Net limit"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());
    }

    void ComponentBase::HandleSpcLimit() {
        riaps_logger_->error("{} was violated, but {} is not implemented in component: {}::{}::{}"
                , "Space limit"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());

    }

    void ComponentBase::HandleNICStateChange(const std::string &state) {
        riaps_logger_->error("{} is not implemented in component: {}::{}::{}"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());
    }

    void ComponentBase::HandlePeerStateChange(const std::string &state, const std::string &uuid) {
        riaps_logger_->error("{} is not implemented in component: {}::{}::{}"
                , __func__
                , actor()->application_name()
                , actor()->actor_name()
                , component_name());
    }

    void ComponentBase::OnScheduledTimer(uint64_t timerId) {
        riaps_logger_->error("Scheduled timer is fired, but no handler is implemented. Implement OnSchedulerTimer() in component {}", component_config().component_name);
    }
    
    void ComponentBase::set_config(component_conf &c_conf) {
        component_config_ = c_conf;
        auto logger_name = fmt::format("::{}", component_config_.component_name);
        riaps_logger_ = spd::get(logger_name);
        if (riaps_logger_ == nullptr)
            riaps_logger_ = spd::stdout_color_mt(logger_name);

        // TODO: pass the debug level by a parameter
        riaps_logger_->set_level(spd::level::debug);

        logger_name = fmt::format("{}",component_config_.component_name);
        component_logger_ = spd::get(logger_name);
        if (component_logger_ == nullptr)
            component_logger_ = spd::stdout_color_mt(logger_name);

    }

    ComponentBase::ComponentBase(const std::string &application_name, const std::string &actor_name) {
        actor_ = std::make_shared<PyActor>(application_name, actor_name);
        component_uuid_ = zuuid_new();
    }

//    ComponentBase::ComponentBase(component_conf& config, Actor& actor)
//            : m_actor(&actor),
//              m_timerCounter(0)
//    {
//        configuration_ = config;
//        setup();
//
//
//        size_t q_size = 2048; //queue size must be power of 2
//        spd::set_async_mode(q_size);
//
//        logger_ = spd::get(configuration_.component_name);
//        if (logger_ == nullptr)
//            logger_ = spd::stdout_color_mt(configuration_.component_name);
//
//
//    }


    ports::PublisherPort* ComponentBase::GetPublisherPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return portBase->AsPublishPort();
    }

    ports::QueryPort* ComponentBase::GetQueryPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return portBase->AsQueryPort();
    }

    ports::RequestPort* ComponentBase::GetRequestPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return portBase->AsRequestPort();
    }

    ports::ResponsePort* ComponentBase::GetResponsePortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return portBase->AsResponsePort();
    }

    ports::SubscriberPort* ComponentBase::GetSubscriberPortByName(const std::string &portName) {
        ports::PortBase* portBase = GetPortByName(portName);
        if (portBase == nullptr) return nullptr;
        return portBase->AsSubscribePort();
    }

    void ComponentBase::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                       capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {
        riaps_logger_->error("Group message arrived, but no handler implemented in the component");
    }

    void ComponentBase::OnMessageToLeader(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& message) {
        riaps_logger_->error("Group message arrived to the leader, but {} is not implemented in component: {}",
                       __FUNCTION__,
                       component_name());
    }

    /// \param portName
    /// \return Pointer to the RIAPS port with the given name. NULL if the port was not found.
    ports::PortBase* ComponentBase::GetPortByName(const std::string & portName) {
        if (ports_.find(portName)!=ports_.end()){
            return ports_[portName].get();
        }

        return nullptr;
    }

    const component_conf& ComponentBase::component_config() const {
        return component_config_;
    }

    void ComponentBase::HandlePortUpdate(const std::string &port_name, const std::string &host,
                                         int port) {
        riaps_logger_->debug("{}({},{},{})", __FUNCTION__, port_name, host, port);
        zmsg_t *msg_portupdate = zmsg_new();

        zmsg_addstr(msg_portupdate, CMD_UPDATE_PORT);
        zmsg_addstr(msg_portupdate, port_name.c_str());
        zmsg_addstr(msg_portupdate, host.c_str());
        zmsg_addstr(msg_portupdate, std::to_string(port).c_str());

        zmsg_send(&msg_portupdate, component_zactor_);
    }

//    const Actor* ComponentBase::GetActor() const{
//        return m_actor;
//    }


    std::shared_ptr<PyActor> ComponentBase::actor() const{
        return actor_;
    }

    const ports::PublisherPort* ComponentBase::InitPublisherPort(const component_port_pub& config) {
        auto result = new ports::PublisherPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        ports_[config.portName] = std::move(newport);
        return result;
    }

    bool ComponentBase::SendMessageOnPort(zmsg_t** message, const std::string& portName) {
        auto port = GetPortByName(portName);
        if (port == nullptr) return false;

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



    const ports::SubscriberPort* ComponentBase::InitSubscriberPort(const component_port_sub& config) {
        std::unique_ptr<ports::SubscriberPort> newport(new ports::SubscriberPort(config, this));
        auto result = newport.get();
        newport->Init();
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::ResponsePort* ComponentBase::InitResponsePort(const component_port_rep & config) {
        auto result = new ports::ResponsePort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::RequestPort*   ComponentBase::InitRequestPort(const component_port_req& config){
        std::unique_ptr<ports::RequestPort> newport(new ports::RequestPort(config, this));
        auto result = newport.get();
        newport->Init();
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::AnswerPort* ComponentBase::InitAnswerPort(const component_port_ans & config) {
        auto result = new ports::AnswerPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::QueryPort* ComponentBase::InitQueryPort(const component_port_qry & config) {
        std::unique_ptr<ports::QueryPort> newport(new ports::QueryPort(config, this));
        auto result = newport.get();
        newport->Init();
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::InsidePort* ComponentBase::InitInsidePort(const component_port_ins& config) {
        auto result = new ports::InsidePort(config, riaps::ports::InsidePortMode::BIND, this);
        std::unique_ptr<ports::PortBase> newport(result);
        ports_[config.portName] = std::move(newport);
        return result;
    }

    const ports::PeriodicTimer* ComponentBase::InitTimerPort(const component_port_tim& config) {
        std::string timerchannel = getTimerChannel();
        std::unique_ptr<ports::PeriodicTimer> newtimer(new ports::PeriodicTimer(timerchannel, config, this));
        newtimer->start();

        auto result = newtimer.get();

        ports_[config.portName] = std::move(newtimer);
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
        if (port == nullptr) return false;

        auto senderPort = dynamic_cast<ports::SenderPort*>(port);
        if (senderPort == nullptr) return false;
        return senderPort->Send(message);
    }

    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder &message, const std::string &portName,
                                          std::shared_ptr<riaps::MessageParams> params) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == nullptr) return false;

        auto answerPort = dynamic_cast<ports::AnswerPort*>(port);
        if (answerPort == nullptr) return false;
        return answerPort->SendAnswer(message, params);
    }

    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder &message, const std::string &portName,
                                          std::string &requestId) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == nullptr) return false;

        ports::QueryPort* queryPort = port->AsQueryPort();
        if (queryPort == nullptr) return false;
        return queryPort->SendQuery(message, requestId);
    }

    bool ComponentBase::SendGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::MallocMessageBuilder &message,
                                         const std::string& portName) {
        // Search the group
        if (groups_.find(groupId)==groups_.end()) return false;

        riaps::groups::Group* group = groups_[groupId].get();

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
        riaps_logger_->debug("Message from the leader arrived, but no OnMessageFromHandler() implementation has found in component: {}", component_config().component_name);
    }

    riaps::groups::Group* ComponentBase::getGroupById(const riaps::groups::GroupId &groupId) {
        if (groups_.find(groupId)==groups_.end()) return nullptr;

        return groups_[groupId].get();
    }


//    const ports::PortBase* ComponentBase::GetPort(std::string portName) const {
//        auto port_it = _ports.find(portName);
//        if (port_it!=_ports.end()){
//            return port_it->second.get();
//        }
//        return nullptr;
//    }


    std::string ComponentBase::getTimerChannel() {
        return fmt::format("inproc://timer{}", ComponentUuid());
    }

    string ComponentBase::getOneShotTimerChannel() {
        return fmt::format("inproc://oneshottimer{}",ComponentUuid());
    }

    const string ComponentBase::ComponentUuid() const{
        string uuid_str = zuuid_str(component_uuid_);
        return uuid_str;
    }

    zactor_t* ComponentBase::GetZmqPipe() const {
        return component_zactor_;
    }

    // TODO: Remove timeout parameter
    uint16_t ComponentBase::GetGroupMemberCount(const riaps::groups::GroupId &groupId, int64_t timeout) {
        if (groups_.find(groupId)==groups_.end())
            return 0;

        return groups_[groupId]->GetMemberCount();
    }

    string ComponentBase::GetLeaderId(const riaps::groups::GroupId &groupId) {
        if (groups_.find(groupId)==groups_.end())
            return "";

        return groups_[groupId]->leader_id();
    }

    bool ComponentBase::JoinGroup(riaps::groups::GroupId &&groupId) {
        return JoinGroup(groupId);
    }

    bool ComponentBase::JoinGroup(riaps::groups::GroupId &groupId) {
        if (groups_.find(groupId)!=groups_.end())
            return false;

//        auto newGroupPtr = new riaps::groups::Group(groupId, this);
//        auto newGroup = unique_ptr<riaps::groups::Group>(
//                newGroupPtr
//        );

        auto new_group = make_unique<riaps::groups::Group>(groupId, this);

        if (new_group->InitGroup()) {
            groups_[groupId] = std::move(new_group);
            return true;
        }

        return false;
    }

    bool ComponentBase::LeaveGroup(riaps::groups::GroupId &&groupId) {
        return JoinGroup(groupId);
    }

    bool ComponentBase::LeaveGroup(riaps::groups::GroupId &groupId) {
        if (groups_.find(groupId) == groups_.end())
            return false;
        groups_.erase(groupId);
        return true;
    }

    std::vector<riaps::groups::GroupId> ComponentBase::GetGroupMemberships() {
        std::vector<riaps::groups::GroupId> results;

        for(auto& group : groups_) {
            results.push_back(group.first);
        }

        return results;
    }

    bool ComponentBase::IsMemberOf(riaps::groups::GroupId &groupId) {
        for(auto& group : groups_) {
            if (group.first == groupId)
                return true;
        }
        return false;
    }

    bool ComponentBase::IsLeader(const riaps::groups::Group* group) {
        return ComponentUuid() == group->leader_id();
    }

    bool ComponentBase::IsLeader(const riaps::groups::GroupId &groupId) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return false;
        return IsLeader(group);
    }

    bool ComponentBase::IsLeaderAvailable(const riaps::groups::GroupId &groupId) {
        if (groups_.find(groupId) == groups_.end())
            return false;
        return !groups_[groupId]->leader_id().empty();
    }

    std::vector<riaps::groups::GroupId> ComponentBase::GetGroupMembershipsByType(const std::string &groupType) {
        std::vector<riaps::groups::GroupId> results;
        for(auto& group : groups_) {
            if (group.first.group_type_id != groupType) continue;
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
        uint64_t timerId = timer_counter_;

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



            clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &earlyWakeup, nullptr);
            zmsg_send(&msg, ptr.get());
            zclock_sleep(400);
        });
        t.detach();
        return timer_counter_++;
    }

    uint64_t ComponentBase::ScheduleAction(const timespec &tp,
                                           std::function<void(const uint64_t)> action,
                                           uint64_t wakeupOffset) {
        std::string timerChannel = getOneShotTimerChannel();
        uint64_t timerId = timer_counter_;
        scheduled_action_ = action;

        // TODO: C++ async
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



            clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &earlyWakeup, nullptr);
            zmsg_send(&msg, ptr.get());
            zclock_sleep(400);
        });
        t.detach();
        return timer_counter_++;
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
        riaps_logger_->error("Vote result is announced, but no handler implemented in component {}", component_name());
    }

    void ComponentBase::OnPropose(riaps::groups::GroupId &groupId, const std::string &proposeId,
                                  capnp::FlatArrayMessageReader &message) {
        riaps_logger_->error("Leader proposed a value but no handler is implemented in component {}", component_name());
    }

    void ComponentBase::OnActionPropose(riaps::groups::GroupId &groupId,
                                        const std::string      &proposeId,
                                        const std::string      &actionId,
                                        const timespec         &timePoint) {
        riaps_logger_->info("Leader proposed an action, but no handler is implemented in component {}", component_name());
    }

    const string ComponentBase::component_name() const {
        return component_config().component_name;
    }

    string ComponentBase::SendPropose(const riaps::groups::GroupId &groupId, capnp::MallocMessageBuilder &message) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return "";

        auto uuid = unique_ptr<zuuid_t, function<void(zuuid_t*)>>(zuuid_new(), [](zuuid_t* u){zuuid_destroy(&u);});
        string strUuid = zuuid_str(uuid.get());

        if (group->ProposeValueToLeader(message, strUuid)){
            return strUuid;
        }
        return "";
    }

    string ComponentBase::ProposeAction(const riaps::groups::GroupId &groupId,
                                        const std::string &actionId,
                                        const timespec &absTime) {
        auto group = getGroupById(groupId);
        if (group == nullptr) return "";

        auto uuid = unique_ptr<zuuid_t, function<void(zuuid_t*)>>(zuuid_new(), [](zuuid_t* u){zuuid_destroy(&u);});
        string strUuid = zuuid_str(uuid.get());

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
        gid.group_name    = msgGroupUpdate.getGroupId().getGroupName().cStr();
        gid.group_type_id = msgGroupUpdate.getGroupId().getGroupType().cStr();

        if (groups_.find(gid) == groups_.end()) return;

        groups_[gid]->ConnectToNewServices(msgGroupUpdate);
    }
}