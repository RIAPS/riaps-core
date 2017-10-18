//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_componentbase.h"


// TODO: Move this somewhere else
#define ASYNC_CHANNEL "ipc://asyncresponsepublisher"

namespace riaps{

    void component_actor(zsock_t* pipe, void* args){
        ComponentBase* comp = (ComponentBase*)args;

        zsock_t* timerport = zsock_new_pull(comp->GetTimerChannel().c_str());
        assert(timerport);

//        Note: Disable one shot timers, we need more tests.
//        zsock_t* timerportOneShot = zsock_new_pull(comp->GetOneShotTimerChannel().c_str());
//        assert(timerportOneShot);

        zpoller_t* poller = zpoller_new(pipe, NULL);
        assert(poller);

        // New api is czmq, ignore_interrupts is obsolote
        zpoller_set_nonstop(poller, true);
        zsock_signal (pipe, 0);

        int rc = zpoller_add(poller, timerport);
        assert(rc==0);

//        rc = zpoller_add(poller, timerportOneShot);
//        assert(rc==0);

        bool terminated = false;
        bool firstrun = true;

        std::cout << "Component poller starts" << std::endl;

        // Register ZMQ Socket - riapsPort pairs. For the quick retrieve.
        std::map<const zsock_t*, const ports::PortBase*>   portSockets;

        // ZMQ Socket - Inside port pairs
        std::map<const zsock_t*, const ports::InsidePort*> insidePorts;

        while (!terminated) {
            void *which = zpoller_wait(poller, 500);

            if (firstrun) {
                firstrun = false;

                const component_conf& comp_conf = comp->GetConfig();

                // Add insider ports
                for (auto it_insconf = comp_conf.component_ports.inss.begin();
                     it_insconf != comp_conf.component_ports.inss.end();
                     it_insconf++){
                    const ports::InsidePort* newPort = comp->InitInsiderPort(*it_insconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    insidePorts[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start timers
                for (auto it_timconf = comp_conf.component_ports.tims.begin();
                          it_timconf != comp_conf.component_ports.tims.end();
                          it_timconf++){
                    // Don't put the zmqSocket into portSockets[zmqSocket], just one timer port exist in the component.
                    // Cannot differentiate timerports based on ZMQ Sockets.
                    comp->InitTimerPort(*it_timconf);
                }

                // Add and start publishers
                for (auto it_pubconf = comp_conf.component_ports.pubs.begin();
                          it_pubconf != comp_conf.component_ports.pubs.end();
                          it_pubconf++){
                    const ports::PublisherPort* newPort = comp->InitPublisherPort(*it_pubconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();

                    portSockets[zmqSocket] = newPort;
                }

                // Add and start response ports
                for (auto it_repconf = comp_conf.component_ports.reps.begin();
                     it_repconf != comp_conf.component_ports.reps.end();
                     it_repconf++){

                    const ports::ResponsePort* newPort = comp->InitResponsePort(*it_repconf);
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

                    const ports::RequestPort* newPort = comp->InitRequestPort(*it_reqconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }

                // Add and start subscribers
                for (auto it_subconf = comp_conf.component_ports.subs.begin();
                          it_subconf != comp_conf.component_ports.subs.end();
                          it_subconf++) {
                    const ports::SubscriberPort* newPort = comp->InitSubscriberPort(*it_subconf);
                    const zsock_t* zmqSocket = newPort->GetSocket();
                    portSockets[zmqSocket] = newPort;
                    zpoller_add(poller, (void*)newPort->GetSocket());
                }
            }

            if (which == pipe) {
                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                char *command = zmsg_popstr(msg);

                if (streq(command, "$TERM")) {
                    std::cout << "$TERM arrived in component" << std::endl;
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
                                    }
                                }

                                zstr_free(&port);
                            }
                            zstr_free(&host);
                        }
                        zstr_free(&portname);
                    } else if(streq(command, CMD_UPDATE_GROUP)){
                        std::string groupTypeId = zmsg_popstr(msg);
                        std::string groupName   = zmsg_popstr(msg);
                        std::string address     = zmsg_popstr(msg);
                        std::string messageType = zmsg_popstr(msg);

                        // Grab the group object
                        riaps::groups::GroupId gid;
                        gid.groupTypeId = groupTypeId;
                        gid.groupName   = groupName;

                        auto groups = &(comp->_groups);

                        if (groups->find(gid)!=groups->end()){


                        }
                    }
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

                    // comp->DispatchMessage(timerPort->GetPortName(), NULL, timerPort);
                    if (!terminated)
                        comp->DispatchMessage(NULL, timerPort);



                    zstr_free(&portName);
                    zmsg_destroy(&msg);
                }
            }
            // One shot timer fired somewhere
//            else if (which == timerportOneShot){
//                zmsg_t *msg = zmsg_recv(which);

//                if (msg) {
//                    char *timerId = zmsg_popstr(msg);
//                    if (timerId){
//                        std::string tid = std::string(timerId);
//                        comp->OnOneShotTimer(tid);
//                        zstr_free(&timerId);
//                    }
//                    zmsg_destroy(&msg);
//                }
//
//                if (comp->_oneShotTimer!=NULL){
//                    comp->_oneShotTimer->stop();
//                    delete comp->_oneShotTimer;
//                    comp->_oneShotTimer = NULL;
//                }
//            }
            else if(which){
                // Message test with more than one field
                zmsg_t *msg = zmsg_recv(which);
                if (msg) {

                    // "which" port is an inside port, dispatch ZMQ message
                    if (insidePorts.find((zsock_t*)which)!=insidePorts.end()){
                        ports::InsidePort* insidePort = (ports::InsidePort*)insidePorts[(zsock_t*)which];

                        if (!terminated)
                            comp->DispatchInsideMessage(msg, insidePort);
                    }

                    // "which" port is not an inside port, dispatch CAPNP message
                    else {
                        ports::PortBase* riapsPort = (ports::PortBase*)portSockets[(zsock_t*)which];

                        // zmsg_op transfers the ownership, the frame is being removed from the zmsg
                        zframe_t* bodyFrame = zmsg_pop(msg);
                        size_t size = zframe_size(bodyFrame);
                        byte* data = zframe_data(bodyFrame);

                        auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

                        capnp::FlatArrayMessageReader capnpReader(capnp_data);

                        if (!terminated)
                            comp->DispatchMessage(&capnpReader, riapsPort);

                        zframe_destroy(&bodyFrame);
                    }




                    zmsg_destroy(&msg);
                }
            }
            else{
                // just poll timeout
            }
        }

//        if (comp->_oneShotTimer!=NULL){
//            comp->_oneShotTimer->stop();
//            delete comp->_oneShotTimer;
//            comp->_oneShotTimer = NULL;
//        }

        zpoller_destroy(&poller);
//        zsock_destroy(&timerportOneShot);
        zsock_destroy(&timerport);
    };

    void ComponentBase::StopComponent() {
        //_execute.store(false);
        // Stop timers
        for (auto& config : GetConfig().component_ports.tims){
            std::cout << "Stop timer: " << config.portName << std::endl;
            _ports[config.portName]->AsTimerPort()->stop();
        }

        zactor_destroy(&_zactor_component);

        // Delete ports
        for (auto it = _ports.begin(); it!=_ports.end(); it++){
            auto port = it->second.release();
            delete port;
        }

        zuuid_destroy(&_component_uuid);
    }

    ComponentBase::ComponentBase(component_conf& config, Actor& actor) : _actor(&actor)//, _oneShotTimer(NULL)
    {
        _configuration = config;

        //uuid to the component instance
        _component_uuid = zuuid_new();

        _zactor_component = zactor_new(component_actor, this);
    }

    // For the timer port
//    void ComponentBase::DispatchMessage(const std::string &messagetype, msgpack::sbuffer* message,
//                                        ports::PortBase *port) {
//        auto handler = GetHandler(port->GetPortName());
//        (this->*handler)(messagetype, message, port);
//    }

    //void ComponentBase::DispatchMessage(const std::string &messagetype, MessageBase* message,
    //                                    ports::PortBase *port) {
    //    auto handler = GetHandler(port->GetPortName());
    //    (this->*handler)(messagetype, message, port);
    //}




//    const ports::CallBackTimer* ComponentBase::InitTimerPort(const _component_port_tim_j& config) {
//        std::string timerchannel = GetTimerChannel();
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

//    bool ComponentBase::CreateOneShotTimer(const std::string &timerid, timespec &wakeuptime) {
//        if (_oneShotTimer!=NULL) return false;
//
//        _oneShotTimer = new timers::OneShotTimer(GetOneShotTimerChannel(),
//                                                 timerid,
//                                                 wakeuptime);
//
//        _oneShotTimer->start();
//        return true;
//    }

    /// \param portName
    /// \return Pointer to the RIAPS port with the given name. NULL if the port was not found.
    ports::PortBase* ComponentBase::GetPortByName(const std::string & portName) {
        if (_ports.find(portName)!=_ports.end()){
            return _ports[portName].get();
        }

        return NULL;
    }

    const component_conf& ComponentBase::GetConfig() const {
        return _configuration;
    }

    const Actor* ComponentBase::GetActor() const{
        return _actor;
    }

    const ports::PublisherPort* ComponentBase::InitPublisherPort(const _component_port_pub& config) {
        auto result = new ports::PublisherPort(config);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

//    bool ComponentBase::SendMessageOnPort(zmsg_t** msg, const std::string& portName) const {
//        auto port = GetPort(portName);
//        if (port == NULL) return false;
//
//        return port->Send(msg);
//    }

//    bool ComponentBase::SendMessageOnPort(std::string message, const std::string& portName){
//        ports::PortBase* port = GetPortByName(portName);
//
//        if (port->AsSubscribePort() == NULL && port->AsTimerPort() == NULL){
//            return port->Send(message);
//        }
//
//        return false;
//
//    }



    const ports::SubscriberPort* ComponentBase::InitSubscriberPort(const _component_port_sub& config) {
        std::unique_ptr<ports::SubscriberPort> newport(new ports::SubscriberPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::ResponsePort* ComponentBase::InitResponsePort(const _component_port_rep & config) {
        auto result = new ports::ResponsePort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::RequestPort*   ComponentBase::InitRequestPort(const _component_port_req& config){
        std::unique_ptr<ports::RequestPort> newport(new ports::RequestPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::InsidePort* ComponentBase::InitInsiderPort(const _component_port_ins& config) {
        auto result = new ports::InsidePort(config, riaps::ports::InsidePortMode::BIND, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::PeriodicTimer* ComponentBase::InitTimerPort(const _component_port_tim& config) {
        std::string timerchannel = GetTimerChannel();
        std::unique_ptr<ports::PeriodicTimer> newtimer(new ports::PeriodicTimer(timerchannel, config));
        newtimer->start();

        auto result = newtimer.get();

        _ports[config.portName] = std::move(newtimer);
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
    bool ComponentBase::SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string &portName) {
        ports::PortBase* port = GetPortByName(portName);
        if (port == NULL) return false;

        ports::SenderPort* senderPort = dynamic_cast<ports::SenderPort*>(port);
        if (senderPort == nullptr) return false;
        return senderPort->Send(message);

//        auto serializedMessage = capnp::messageToFlatArray(message);
//        zmsg_t* msg = zmsg_new();
//        auto bytes = serializedMessage.asBytes();
//        zmsg_pushmem(msg, bytes.begin(), bytes.size());
//        return SendMessageOnPort(&msg, portName);
    }

    bool ComponentBase::SendGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::MallocMessageBuilder &message,
                                         const std::string& portName) {
        // Search the group
        if (_groups.find(groupId)==_groups.end()) return false;

        riaps::groups::Group* group = _groups[groupId].get();
        group->SendMessage(message, portName);

    }


    const ports::PortBase* ComponentBase::GetPort(std::string portName) const {
        auto port_it = _ports.find(portName);
        if (port_it!=_ports.end()){
            return port_it->second.get();
        }
        return NULL;
    }


    std::string ComponentBase::GetTimerChannel() {
        std::string prefix= "inproc://timer";
        return prefix + GetCompUuid();
    }

//    std::string ComponentBase::GetOneShotTimerChannel() {
//        std::string prefix= "inproc://oneshottimer";
//        return prefix + GetCompUuid();
//    }

    std::string ComponentBase::GetCompUuid(){
        std::string uuid_str = zuuid_str(_component_uuid);
        //std::string result (uuid_str);
        return uuid_str;
    }

    zactor_t* ComponentBase::GetZmqPipe() const {
        return _zactor_component;
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
        auto parameters = _configuration.component_parameters.GetParameterNames();
        for (auto it = parameters.begin(); it!=parameters.end(); it++){
            std::cout << *it << " : " << _configuration.component_parameters.GetParam(*it)->GetValueAsString() << std::endl;
        }
    }

    bool ComponentBase::JoinToGroup(riaps::groups::GroupId &&groupId) {
        if (_groups.find(groupId)!=_groups.end())
            return false;

        std::unique_ptr<riaps::groups::Group> newGroup = std::unique_ptr<riaps::groups::Group>(new riaps::groups::Group(groupId, GetCompUuid()));
        if (newGroup->InitGroup()) {
            _groups[groupId] = std::move(newGroup);
            return true;
        }

        return false;
    }

    void ComponentBase::UpdateGroup(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
        // First, find the affected groups
        riaps::groups::GroupId gid;
        gid.groupName   = msgGroupUpdate.getGroupId().getGroupName().cStr();
        gid.groupTypeId = msgGroupUpdate.getGroupId().getGroupType().cStr();

        if (_groups.find(gid) == _groups.end()) return;

        _groups[gid]->ConnectToNewServices(msgGroupUpdate);
    }

    ComponentBase::~ComponentBase() {

    }

}