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

        zpoller_t* poller = zpoller_new(pipe, NULL);
        assert(poller);

        zsock_signal (pipe, 0);

        int rc = zpoller_add(poller, timerport);
        assert(rc==0);

        bool terminated = false;
        bool firstrun = true;

        std::cout << "Component poller starts" << std::endl;

        // Register ZMQ Socket - riapsPort pairs. For the quick retrieve.
        std::map<const zsock_t*, const ports::PortBase*> portSockets;

        while (!terminated) {
            void *which = zpoller_wait(poller, 500);

            if (firstrun) {
                firstrun = false;

                // Add and start timers
                for (auto config : comp->GetConfig().component_ports.tims) {
                    // Don't put the zmqSocket into portSockets[zmqSocket], just one timer port exist in the component.
                    // Cannot differentiate timerports based on ZMQ Sockets.
                    comp->InitTimerPort(config);
                }

                // Add and start publishers
                const component_conf_j& comp_conf = comp->GetConfig();
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
                    }
                }

                zstr_free(&command);
                zmsg_destroy(&msg);
            }
            else if (which == timerport) {
                zmsg_t *msg = zmsg_recv(which);

                if (msg) {
                    char *portName = zmsg_popstr(msg);
                    ports::CallBackTimer* timerPort = (ports::CallBackTimer*)comp->GetPortByName(portName);
                    comp->OnMessageArrived(portName, NULL, timerPort);
                    zstr_free(&portName);
                    zmsg_destroy(&msg);
                }
            }
            else if(which){
                // Message test with more than one field
                zmsg_t *msg = zmsg_recv(which);

                if (msg) {
                    ports::PortBase* riapsPort = (ports::PortBase*)portSockets[(zsock_t*)which];
                    const std::string& portName = riapsPort->GetPortName();

                    comp->OnMessageArrived(portName, msg, riapsPort);
                    zmsg_destroy(&msg);
                }
            }
            else{
                // just poll timeout
            }
        }

        // Stop timers
        //for (auto timer : comp->GetPeriodicTimers()){
         //   timer->stop();
        //}

        zsock_destroy(&timerport);
        //zsock_destroy(&asyncport);
        zpoller_destroy(&poller);
    };

    ComponentBase::ComponentBase(component_conf_j& config, Actor& actor) : _actor(&actor) {
        _configuration = config;




        //async uuid to the component instance
        _component_uuid = zuuid_new();

        //get_servicebyname_poll_async("aaa", GetCompUuid());


        //zsock_component = zsock_new_rep("tcp://*:!");
        //assert(zsock_component);

        //async_address = "tcp://192.168.1.103:14352";
        //async_address = "";
        //_zsock_timer = zsock_new_pull(CHAN_TIMER_INPROC);
        //assert(_zsock_timer);

        //zpoller = zpoller_new(zsock_component, NULL);
        //assert(zpoller);

        _zactor_component = zactor_new(component_actor, this);

        // Create publishers
        //for (auto publisher : config.publishers_config) {
        //    AddPublisherPort(publisher);
        //}

        // Create subscribers
        //for (auto subscriber : config.subscribers_config) {
        //    AddSubscriberPort(subscriber);
        //}


    }

    //const zsock_t* ComponentBase::GetTimerPort() {
    //    return _zsock_timer;
    //}

    const ports::PublisherPort* ComponentBase::InitPublisherPort(const _component_port_pub_j& config) {
        auto result = new ports::PublisherPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }



    const ports::SubscriberPort* ComponentBase::InitSubscriberPort(const _component_port_sub_j& config) {
        std::unique_ptr<ports::SubscriberPort> newport(new ports::SubscriberPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::ResponsePort* ComponentBase::InitResponsePort(const _component_port_rep_j & config) {
        auto result = new ports::ResponsePort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::RequestPort*   ComponentBase::InitRequestPort(const _component_port_req_j& config){
        std::unique_ptr<ports::RequestPort> newport(new ports::RequestPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::CallBackTimer* ComponentBase::InitTimerPort(const _component_port_tim_j& config) {
        std::string timerchannel = GetTimerChannel();
        std::unique_ptr<ports::CallBackTimer> newtimer(new ports::CallBackTimer(timerchannel, config));
        newtimer->start(config.period);

        auto result = newtimer.get();

        _ports[config.portName] = std::move(newtimer);
        return result;
    }

    /*
    void ComponentBase::AddResponsePort(std::unique_ptr<ResponsePort>& responsePort) {
        //_responseports.push_back(std::move(responsePort));
    }

    void ComponentBase::AddRequestPort(std::unique_ptr<RequestPort>& requestPort) {
        //_requestports.push_back(std::move(requestPort));
    }

    */

    /// \param portName
    /// \return Pointer to the RIAPS port with the given name. NULL if the port was not found.
    ports::PortBase* ComponentBase::GetPortByName(const std::string & portName) {
        if (_ports.find(portName)!=_ports.end()){
            return _ports[portName].get();
        }

        return NULL;
    }




    const component_conf_j& ComponentBase::GetConfig() const {
        return _configuration;
    }

    const Actor* ComponentBase::GetActor() const{
        return _actor;
    }

    bool ComponentBase::SendMessageOnPort(zmsg_t** msg, const std::string& portName) const {
        auto port = GetPort(portName);
        if (port == NULL) return false;

        return port->Send(msg);
    }

    bool ComponentBase::SendMessageOnPort(const std::string message, std::string portName) const{
        zmsg_t* msg = zmsg_new();
        zmsg_addstr(msg, message.c_str());

        return SendMessageOnPort(&msg, portName);

    }

    const ports::PortBase* ComponentBase::GetPort(std::string portName) const {
        auto port_it = _ports.find(portName);
        if (port_it!=_ports.end()){
            return port_it->second.get();
        }
        return NULL;
    }


    std::vector<ports::PublisherPort*> ComponentBase::GetPublisherPorts() {

        std::vector<ports::PublisherPort*> results;

        // Fixme
        throw std::runtime_error("Not implemented");

        //for (auto it=_publisherports.begin(); it!=_publisherports.end(); it++){
        //    results.push_back(it->second.get());
        //}

        return results;
    }

    std::string ComponentBase::GetTimerChannel() {
        std::string prefix= "inproc://timer";
        return prefix + GetCompUuid();
    }

    std::string ComponentBase::GetCompUuid(){
        const char* uuid_str = zuuid_str(_component_uuid);
        std::string result (uuid_str);
        //delete uuid_str;

        return result;
    }

    zactor_t* ComponentBase::GetZmqPipe() const {
        return _zactor_component;
    }


/*
    std::vector<RequestPort*> ComponentBase::GetRequestPorts() {

        std::vector<RequestPort*> results;

        //for (auto it=_requestports.begin(); it!=_requestports.end(); it++){
        //    results.push_back(it->get());
        //}

        return results;
    }

    std::vector<CallBackTimer*> ComponentBase::GetPeriodicTimers() {
        std::vector<CallBackTimer*> results;

        //for (auto it=_periodic_timers.begin(); it!=_periodic_timers.end(); it++){
        //    results.push_back(it->get());
        //}

        return results;
    }







    std::vector<SubscriberPort*> ComponentBase::GetSubscriberPorts() {

        std::vector<SubscriberPort*> results;

        //for (auto it=_subscriberports.begin(); it!=_subscriberports.end(); it++){
        //    results.push_back(it->get());
        //}

        return results;
    }*/



    ComponentBase::~ComponentBase() {


        zmsg_t* termmsg = zmsg_new();

        zmsg_addstr(termmsg,"$TERM");
        zactor_send(_zactor_component, &termmsg);

        zclock_sleep(1000);

        zuuid_destroy(&_component_uuid);
        //zsock_destroy(&zsock_component);
        //zsock_destroy(&_zsock_timer);
        zactor_destroy(&_zactor_component);

    }

}