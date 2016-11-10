//
// Created by parallels on 9/6/16.
//


#include "componentmodel/r_actor.h"


namespace riaps {

    riaps::Actor::Actor(std::string     applicationname       ,
                        std::string     actorname             ,
                        nlohmann::json& json_actorconfig      ,
                        nlohmann::json& json_componentsconfig ,
                        nlohmann::json& json_messagesconfig)
    {
        _actor_name       = actorname;
        _application_name = applicationname;

        auto json_instances = json_actorconfig["instances"];
        auto json_internals = json_actorconfig["internals"];
        auto json_locals    = json_actorconfig["locals"];
        auto json_wires     = json_actorconfig["wires"];


        // Get the actor's components, if there is no component => Stop, Error.
        if(json_instances.size()==0) {
            throw std::invalid_argument("No component instances defined for the actor. Check the configuration file!");
        }


        // Get the components
        for (auto it_currentconfig =  json_instances.begin();
                  it_currentconfig != json_instances.end();
                  it_currentconfig++) {

            auto componentName = it_currentconfig.key();
            std::string componentType = (it_currentconfig.value())["type"];

            // Check if the component in the map already (wrong configuration)
            for (auto component_config : _component_configurations) {

                if (componentName != component_config.component_name) continue;

                // The same key already in the list. Stop, Error.
                throw std::invalid_argument("Component name is not unique in the configuration file");
            }

            // Store the componentname - componenttype pair
            _component_conf_j new_component_config;

            new_component_config.component_name = componentName;
            new_component_config.component_type = componentType;

            // Get the details of the component
            auto json_componentconfig = json_componentsconfig[componentType];

            // Get the ports
            auto json_portsconfig = json_componentconfig["ports"];

            // Get the publishers
            if (json_portsconfig.count("pubs")!=0){
                auto json_pubports = json_portsconfig["pubs"];
                for (auto it_pubport=json_pubports.begin();
                          it_pubport!=json_pubports.end();
                          it_pubport++){

                    auto pubportname = it_pubport.key();
                    auto pubporttype = it_pubport.value()["type"];

                    _component_port_pub_j newpubconfig;
                    newpubconfig.publisher_name = pubportname;
                    newpubconfig.message_type = pubporttype;

                    new_component_config.component_ports.pubs.push_back(newpubconfig);
                }
            }

            _component_configurations.push_back(new_component_config);
        }
    }

    void riaps::Actor::Init() {

        // unique id / run
        _actor_id = zuuid_new();

        // Open actor REP socket for further communications
        _actor_zsock = zsock_new_rep("tcp://*:!");
        assert(_actor_zsock);
        _poller = zpoller_new(_actor_zsock, NULL);
        assert(_poller);

        // Register the actor in the discovery service
        zsock_t* discovery_socket = register_actor(_application_name, _actor_name);

        if (discovery_socket == NULL) {
            throw std::runtime_error("Actor - Discovery socket cannot be NULL after register_actor");
        }

        // If no component in the actor => Stop, Error
        if (_component_configurations.empty()){
            throw std::invalid_argument("No components are defined in the configuration file.");
        }

        for (auto component_config : _component_configurations){
            // Load the component library

            std::string component_library_name = "./lib" + component_config.component_name + ".so";
            void *handle = dlopen(component_library_name.c_str(), RTLD_NOW);
            if (handle == NULL) {
                throw std::runtime_error("Error when opening library: " + component_library_name + " (" + dlerror() + ")");
            }
            else {
                _component_dll_handles.push_back(handle);
                riaps::ComponentBase * (*create)(component_conf&);
                create = (riaps::ComponentBase *(*)(component_conf&)) dlsym(handle, "create_component");
                riaps::ComponentBase* component_instance = (riaps::ComponentBase *)create(cconf);
                _components.push_back(component_instance);
                //riaps::ComponentBase *component_instance = (riaps::ComponentBase *) create(cconf);
            }
        }
    }

    std::string riaps::Actor::GetActorId() {
        return std::string(zuuid_str(_actor_id));

    }

    void riaps::Actor::start(){
        /*std::ifstream ifs(configfile);

        std::vector<std::unique_ptr<riaps::ComponentBase>> components;

        if (ifs.good()){

            nlohmann::json config_json = nlohmann::json::parse(ifs);

            auto actor_config      = config_json["actor"];
            auto components_config = config_json["components"];



            // Get the component configs
            for (auto itcomp = components_config.begin(); itcomp != components_config.end(); ++itcomp) {
                nlohmann::json current_component = (*itcomp);

                component_conf cconf;

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
         */
    }

    riaps::Actor::~Actor() {
        //deregister_actor(GetActorId());
        zuuid_destroy(&_actor_id);
        zsock_destroy(&_discovery_socket);
        zsock_destroy(&_actor_zsock);
        zpoller_destroy(&_poller);

        for (auto handle : _component_dll_handles){
            dlclose(handle);
        }
    }
}


