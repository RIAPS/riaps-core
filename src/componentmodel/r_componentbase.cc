//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_componentbase.h"


// TODO: Move this somewhere else
#define ASYNC_CHANNEL "ipc://asyncresponsepublisher"

namespace riaps{

    void component_actor(zsock_t* pipe, void* args){
        ComponentBase* comp = (ComponentBase*)args;

        //zsock_t* timerport = (zsock_t*)comp->GetTimerPort()
        //std::cout << "Create async endpoint @" << comp->GetAsyncEndpointName() << std::endl;
        zsock_t* asyncport = zsock_new_sub(ASYNC_CHANNEL, comp->GetCompUuid().c_str());
        assert(asyncport);

        zpoller_t* poller = zpoller_new(pipe, NULL);
        assert(poller);

        zsock_signal (pipe, 0);

        int rc = zpoller_add(poller, asyncport);
        assert(rc==0);

        // Init subscribers

        // TODO: Uncommented for debug, PUT IT BACK!
        //for (auto subscriberport : comp->GetSubscriberPorts()) {
        //    const zsock_t* socket = subscriberport->GetSocket();
        //    zpoller_add(poller, (zsock_t*)socket);
        //}

        bool terminated = false;
        bool firstrun = true;
        while (!terminated) {
            void *which = zpoller_wait(poller, 1000);
            std::cout << "Component poller running" << std::endl;

            if (firstrun) {
                firstrun = false;
                // Add publishers
                for (auto publisher : comp->GetConfig().publishers_config) {
                    comp->AddPublisherPort(publisher);
                }

                // Get subscriber details from discovery service
                for (auto subscriber : comp->GetConfig().subscribers_config) {
                    SubscriberPort::GetRemoteServiceAsync(subscriber, comp->GetCompUuid());
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
                }

                free(command);
                zmsg_destroy(&msg);
            }
            /*else if (which == timerport) {
                std::cout << "Timer?" << std::endl;
                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                char *param = zmsg_popstr(msg);

                if (param){
                    std::cout << param << std::endl;
                    free (param);

                }

            }*/
            // Message from async query
            else if (which == asyncport){
                std::cout<< "ASYNC ARRIVED, Create subscriber and connect" << std::endl;
                std::vector<service_details> services;
                zmsg_t *msg_response = zmsg_recv(which);

                if (!msg_response){
                    std::cout << "No msg => interrupted" << std::endl;
                    continue;
                }

                // The header must contain the UUID
                char* msg_header = zmsg_popstr(msg_response);

                // Todo, wrong message => log it
                if (strcmp(msg_header, comp->GetCompUuid().c_str())){
                    zmsg_destroy(&msg_response);
                    free(msg_header);
                    continue;
                }

                free(msg_header);

                std::vector<zmsg_t*> responseframes;
                extract_zmsg(msg_response, responseframes);

                for (auto it = responseframes.begin(); it!=responseframes.end(); it++) {
                    std::vector<std::string> params;
                    service_details current_service;
                    extract_params(*it, params);
                    params_to_service_details(params, current_service);
                    services.push_back(current_service);
                    zmsg_destroy(&(*it));
                }

                zmsg_destroy(&msg_response);

                if (!services.empty()) {
                    service_details target_service = services.front();
                    auto subscriberport = SubscriberPort::InitFromServiceDetails(target_service);
                    auto zmqport = subscriberport->GetSocket();
                    comp->AddSubscriberPort(subscriberport);

                    rc = zpoller_add(poller, (zsock_t*)zmqport);
                    assert(rc==0);
                }
            }
            // TODO: from one of the publishers?
            else if(which){
                // Message test with more than one field
                zmsg_t *msg = zmsg_recv(which);

                if (msg) {
                    char* messagetype = zmsg_popstr(msg);

                    comp->OnMessageArrived(std::string(messagetype), msg);

                    free(messagetype);
                }
            }
            else{
                // just poll timeout
            }
        }

        zsock_destroy(&asyncport);
        zpoller_destroy(&poller);
    };

    ComponentBase::ComponentBase(component_conf& config) {
        configuration = config;

        //async uuid to the component instance
        component_uuid = zuuid_new();

        //zsock_component = zsock_new_rep("tcp://*:!");
        //assert(zsock_component);

        //async_address = "tcp://192.168.1.103:14352";
        //async_address = "";
        //_zsock_timer = zsock_new_pull(CHAN_TIMER_INPROC);
        //assert(_zsock_timer);

        //zpoller = zpoller_new(zsock_component, NULL);
        //assert(zpoller);

        zactor_component = zactor_new(component_actor, this);

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

    void ComponentBase::AddPublisherPort(publisher_conf& config) {
        std::unique_ptr<PublisherPort> newport(new PublisherPort(config));
        _publisherports.push_back(std::move(newport));
    }

    void ComponentBase::AddSubscriberPort(std::unique_ptr<SubscriberPort>& subscriberport) {
        _subscriberports.push_back(std::move(subscriberport));
    }

    component_conf& ComponentBase::GetConfig() {
        return configuration;
    }

    std::vector<PublisherPort*> ComponentBase::GetPublisherPorts() {

        std::vector<PublisherPort*> results;

        for (auto it=_publisherports.begin(); it!=_publisherports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    std::string ComponentBase::GetCompUuid(){
        return std::string(zuuid_str(component_uuid));
    }

    std::vector<SubscriberPort*> ComponentBase::GetSubscriberPorts() {

        std::vector<SubscriberPort*> results;

        for (auto it=_subscriberports.begin(); it!=_subscriberports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    ComponentBase::~ComponentBase() {
        zuuid_destroy(&component_uuid);
        //zsock_destroy(&zsock_component);
        //zsock_destroy(&_zsock_timer);
        zactor_destroy(&zactor_component);
    }

}