//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_componentbase.h"

namespace riaps{

    void component_actor(zsock_t* pipe, void* args){
        ComponentBase* comp = (ComponentBase*)args;



        zpoller_t* poller = zpoller_new(pipe, NULL);



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
            } else {

            }
        }

        zpoller_destroy(&poller);
    };

    ComponentBase::ComponentBase(component_conf& config) {
        zsock_component = zsock_new_rep("tcp://*:!");
        assert(zsock_component);
        zpoller = zpoller_new(zsock_component);
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

    void ComponentBase::AddPublisherPort(publisher_conf& config) {
        std::unique_ptr<PublisherPort> newport(new PublisherPort(config));
        publisherports.push_back(std::move(newport));
    }

    void ComponentBase::AddSubscriberPort(subscriber_conf& config) {
        subscriberports.push_back(std::unique_ptr<SubscriberPort>(new SubscriberPort(config)));
    }

    std::vector<PublisherPort*> ComponentBase::GetPublisherPorts() {

        std::vector<PublisherPort*> results;

        for (auto it=publisherports.begin(); it!=publisherports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    std::vector<SubscriberPort*> ComponentBase::GetSubscriberPorts() {

        std::vector<SubscriberPort*> results;

        for (auto it=subscriberports.begin(); it!=subscriberports.end(); it++){
            results.push_back(it->get());
        }

        return results;
    }

    ComponentBase::~ComponentBase() {
        zsock_destroy(&zsock_component);
        zactor_destroy(&zactor_component);
    }

}