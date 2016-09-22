//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_componentbase.h"

namespace riaps{

    void component_actor(zsock_t* pipe, void* args){
        ComponentBase* comp = (ComponentBase*)args;


        //zsock_t* timerport = (zsock_t*)comp->GetTimerPort();
        zsock_t* asyncport = zsock_new_pull("ipc://async");
        assert(asyncport);

        zpoller_t* poller = zpoller_new(pipe, NULL);

        // TODO: Uncommented for debug, PUT IT BACK!
        //for (auto subscriberport : comp->GetSubscriberPorts()) {
        //    const zsock_t* socket = subscriberport->GetSocket();
        //    zpoller_add(poller, (zsock_t*)socket);
        //}

        assert(poller);

        zsock_signal (pipe, 0);

        bool terminated = false;
        while (!terminated) {
            void *which = zpoller_wait(poller, 1000);
            std::cout << "Component poller running" << std::endl;
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
            //else if (which == asyncport){
            //    std::cout<< "YEAH ASYNC ARRIVED" << std::endl;
//
             //   zmsg_t *msg = zmsg_recv(which);
             //   zmsg_destroy(&msg);
            //}
            // TODO: from one of the subscribers?
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
        zsock_component = zsock_new_rep("tcp://*:!");
        assert(zsock_component);

        //_zsock_timer = zsock_new_pull(CHAN_TIMER_INPROC);
        //assert(_zsock_timer);



        zpoller = zpoller_new(zsock_component, NULL);
        assert(zpoller);

        configuration = config;

        // Create publishers
        for (auto publisher : config.publishers_config) {
            AddPublisherPort(publisher);
        }

        // Create subscribers
        for (auto subscriber : config.subscribers_config) {
            AddSubscriberPort(subscriber);
        }

        zactor_component = zactor_new(component_actor, this);
    }

    //const zsock_t* ComponentBase::GetTimerPort() {
    //    return _zsock_timer;
    //}

    void ComponentBase::AddPublisherPort(publisher_conf& config) {
        std::unique_ptr<PublisherPort> newport(new PublisherPort(config));
        _publisherports.push_back(std::move(newport));
    }

    void ComponentBase::AddSubscriberPort(subscriber_conf& config) {
        _subscriberports.push_back(std::unique_ptr<SubscriberPort>(new SubscriberPort(config)));
    }

    std::vector<PublisherPort*> ComponentBase::GetPublisherPorts() {

        std::vector<PublisherPort*> results;

        for (auto it=_publisherports.begin(); it!=_publisherports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    std::vector<SubscriberPort*> ComponentBase::GetSubscriberPorts() {

        std::vector<SubscriberPort*> results;

        for (auto it=_subscriberports.begin(); it!=_subscriberports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    ComponentBase::~ComponentBase() {
        zsock_destroy(&zsock_component);
        //zsock_destroy(&_zsock_timer);
        zactor_destroy(&zactor_component);
    }

}