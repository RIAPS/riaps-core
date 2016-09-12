/** @file    actor.cpp 
 *  @author  Pranav Srinivas Kumar, Istvan Madari
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Actor class
 */

#include "../../include/zcm/actor.hpp"

namespace zcm {

    // Use configuration file to prepare component instance vector
    void Actor::configure(std::string configuration_file) {
        Json::Value root;
        std::ifstream configuration(configuration_file, std::ifstream::binary);
        configuration >> root;

        zmq::context_t *context = new zmq::context_t(1);

        for (unsigned int i = 0; i < root["Component Instances"].size(); i++) {
            std::string component_library = root["Component Instances"][i]["Definition"].asString();
            void *handle = dlopen(("lib" + component_library).c_str(), RTLD_NOW);
            if (handle == NULL)
                std::cerr << dlerror() << std::endl;
            else {
                Component *(*create)();
                create = (Component *(*)()) dlsym(handle, "create_component");
                Component *component_instance = (Component *) create();

                std::map<std::string, std::vector<zcm_endpoint>> publishers_config_map;
                std::map<std::string, std::vector<std::string>> subscribers_config_map;
                std::map<std::string, std::vector<std::string>> clients_config_map;
                std::map<std::string, std::vector<std::string>> servers_config_map;

                // Configure all component instance timers
                for (unsigned int j = 0; j < root["Component Instances"][i]["Timers"].size(); j++) {
                    Json::Value timer_config = root["Component Instances"][i]["Timers"][j];
                    std::string timer_name = timer_config["Name"].asString();
                    unsigned int timer_priority = timer_config["Priority"].asUInt();
                    long long timer_period = (long long) (1000000000 * timer_config["Period"].asFloat());
                    std::string timer_operation = timer_config["Function"].asString();
                    Timer *new_timer = new Timer(timer_name,
                                                 timer_priority,
                                                 timer_period,
                                                 component_instance->functionality[timer_operation],
                                                 component_instance->get_operation_queue());
                    component_instance->add_timer(new_timer);
                }

                // Configure all component instance publishers
                for (unsigned int j = 0; j < root["Component Instances"][i]["Publishers"].size(); j++) {
                    Json::Value publisher_config = root["Component Instances"][i]["Publishers"][j];
                    std::string publisher_name = publisher_config["Name"].asString();
                    for (unsigned int k = 0; k < publisher_config["Endpoints"].size(); k++) {
                        zcm_endpoint conf;
                        conf.address = publisher_config["Endpoints"][k]["address"].asString();
                        conf.port = publisher_config["Endpoints"][k]["port"].asString();
                        publishers_config_map[publisher_name].push_back(conf);
                    }
                    Publisher *new_publisher = new Publisher(publisher_name, context);
                    component_instance->add_publisher(new_publisher);
                }

                // Configure all component instance subscribers
                for (unsigned int j = 0; j < root["Component Instances"][i]["Subscribers"].size(); j++) {
                    Json::Value subscriber_config = root["Component Instances"][i]["Subscribers"][j];
                    std::string subscriber_name = subscriber_config["Name"].asString();
                    unsigned int subscriber_priority = subscriber_config["Priority"].asUInt();
                    std::string subscriber_filter = subscriber_config["Filter"].asString();
                    std::string subscriber_operation = subscriber_config["Function"].asString();

                    // TODO: Do we want to use endpoints? I don't think so. Discvoery service replaces this.
                    // TODO: But, de we want to use the system without discovery service?
                    // TODO: Who knows, probably not... But ask it on the meeting
                    //for (unsigned int k = 0; k < subscriber_config["Endpoints"].size(); k++) {
                    //    subscribers_config_map[subscriber_name].push_back(subscriber_config["Endpoints"][k].asString());
                    //}


                    for (int i = 0; i<subscriber_config["PublisherNames"].size(); i++){
                        subscribers_config_map[subscriber_name].push_back(subscriber_config["PublisherNames"][i].asString());

                        // TODO: expect more than 1 publisher
                        break;
                    }

                    Subscriber *new_subscriber = new Subscriber(subscriber_name,
                                                                subscriber_priority,
                                                                context,
                                                                subscriber_filter,
                                                                component_instance->
                                                                        functionality[subscriber_operation],
                                                                component_instance->get_operation_queue());
                    component_instance->add_subscriber(new_subscriber);
                }

                // Configure all component instance clients
                for (unsigned int j = 0; j < root["Component Instances"][i]["Clients"].size(); j++) {
                    Json::Value client_config = root["Component Instances"][i]["Clients"][j];
                    std::string client_name = client_config["Name"].asString();
                    int client_timeout = (int) (1000 * client_config["Timeout"].asFloat());
                    for (unsigned int k = 0; k < client_config["Endpoints"].size(); k++) {
                        clients_config_map[client_name].push_back(client_config["Endpoints"][k].asString());
                    }
                    Client *new_client = new Client(client_name, context, client_timeout);
                    component_instance->add_client(new_client);
                }

                // Configure all component instance servers
                for (unsigned int j = 0; j < root["Component Instances"][i]["Servers"].size(); j++) {
                    Json::Value server_config = root["Component Instances"][i]["Servers"][j];
                    int server_timeout = (int) (1000 * server_config["Timeout"].asFloat());
                    std::string server_name = server_config["Name"].asString();
                    unsigned int server_priority = server_config["Priority"].asUInt();
                    std::string server_operation = server_config["Function"].asString();
                    for (unsigned int k = 0; k < server_config["Endpoints"].size(); k++) {
                        servers_config_map[server_name].push_back(server_config["Endpoints"][k].asString());
                    }
                    Server *new_server = new Server(server_name,
                                                    server_priority,
                                                    context,
                                                    component_instance->functionality[server_operation],
                                                    component_instance->get_operation_queue(),
                                                    server_timeout);
                    component_instance->add_server(new_server);
                }

                component_instance->configure_publishers(publishers_config_map);
                component_instance->configure_subscribers(subscribers_config_map);
                component_instance->configure_clients(clients_config_map);
                component_instance->configure_servers(servers_config_map);
                component_instances.push_back(component_instance);
            }
        }
    }

    void Actor::run() {
        std::vector<std::thread *> component_instance_threads;
        for (auto instance : component_instances) {
            std::thread *instance_thread = instance->spawn();
            component_instance_threads.push_back(instance_thread);
        }

        for (auto instance : component_instance_threads) {
            instance->join();
        }
    }

    std::string Actor::get_name() {
        return name;
    }

    std::string Actor::get_configuration() {
        return configuration;
    }

    pid_t Actor::get_pid() {
        return pid;
    }

    void Actor::set_pid(pid_t new_pid) {
        pid = new_pid;
    }

}
