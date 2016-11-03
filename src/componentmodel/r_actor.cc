//
// Created by parallels on 9/6/16.
//


#include "componentmodel/r_actor.h"


namespace riaps {
    riaps::Actor::Actor(std::string actorid):_actor_id(actorid) {
        _actor_zsock = zsock_new_rep("tcp://*:!");
        assert(_actor_zsock);
        _poller = zpoller_new(_actor_zsock, NULL);
        assert(_poller);

        register_actor(_actor_id);
    }

    void riaps::Actor::start(std::string configfile){
        std::ifstream ifs(configfile);

        std::vector<std::unique_ptr<riaps::ComponentBase>> components;

        if (ifs.good()){

            nlohmann::json config_json = nlohmann::json::parse(ifs);

            auto actor_config      = config_json["actor"];
            auto components_config = config_json["components"];

            component_conf cconf;

            // Get the component configs
            for (auto itcomp = components_config.begin(); itcomp != components_config.end(); ++itcomp) {
                nlohmann::json current_component = (*itcomp);

                auto publishers  = current_component["publishers"];
                auto subscribers = current_component["subscribers"];
                auto timers      = current_component["timers"];
                auto clients     = current_component["clients"];
                auto servers     = current_component["servers"];

                cconf.component_name = current_component["name"];

                // Get the publishers
                for (auto itpub = publishers.begin(); itpub != publishers.end(); ++itpub) {
                    nlohmann::json current_publisher = (*itpub);

                    // Publisher port
                    publisher_conf pport;
                    pport.servicename = current_publisher["name"];
                    pport.network_iface = current_publisher["network_iface"];
                    pport.port = current_publisher["port"];

                    cconf.publishers_config.push_back(pport);
                }

                // Get the timers
                for (auto ittim = timers.begin(); ittim != timers.end(); ++ittim) {
                    nlohmann::json current_timer = (*ittim);
                    periodic_timer_conf ptimer;
                    ptimer.timerid   = current_timer["timerid"];
                    ptimer.interval  = current_timer["interval"];
                    cconf.periodic_timer_config.push_back(ptimer);
                }

                // Get the subscribers
                for (auto itsub = subscribers.begin(); itsub != subscribers.end(); ++itsub) {
                    nlohmann::json current_subscriber = (*itsub);

                    subscriber_conf sconf;
                    sconf.remoteservice_name = current_subscriber["remoteservice_name"];
                    sconf.servicename = current_subscriber["name"];       // Optional parameter
                    cconf.subscribers_config.push_back(sconf);
                }

                // Get the clients (request ports)
                for (auto itcli = clients.begin(); itcli != clients.end(); ++itcli) {
                    nlohmann::json current_client = (*itcli);
                    request_conf rconf;
                    rconf.remoteservice_name = current_client["remoteservice_name"];
                    cconf.requests_config.push_back(rconf);
                }

                // Get the server (response ports)
                for (auto itsrv = servers.begin(); itsrv != servers.end(); ++itsrv) {
                    nlohmann::json current_server = (*itsrv);
                    response_conf resp_conf;
                    resp_conf.network_iface = current_server["network_iface"];
                    resp_conf.port = current_server["port"]; // auto bind
                    resp_conf.servicename = current_server["name"];;
                }

                void *handle = dlopen(("./lib" + cconf.component_name + ".so").c_str(), RTLD_NOW);
                if (handle == NULL)
                    std::cerr << dlerror() << std::endl;
                else {
                    riaps::ComponentBase * (*create)(component_conf&);
                    create = (riaps::ComponentBase *(*)(component_conf&)) dlsym(handle, "create_component");
                    riaps::ComponentBase* component_instance = (riaps::ComponentBase *)create(cconf);
                    components.push_back(std::unique_ptr<riaps::ComponentBase>(component_instance));
                    //riaps::ComponentBase *component_instance = (riaps::ComponentBase *) create(cconf);
                }

            }

            while (!zsys_interrupted) {
                void *which = zpoller_wait(_poller, 2000);

                if (which == _actor_zsock) {
                    // Maybe later, control messages to the actor
                } else {
                }
            }
        }
    }

    riaps::Actor::~Actor() {
        deregister_actor(_actor_id);
        zsock_destroy(&_actor_zsock);
        zpoller_destroy(&_poller);
    }
}


