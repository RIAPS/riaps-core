/** @file    component.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Component class
 */

#include "../../include/zcm/component.hpp"

namespace zcm {

    // Construct the operation queue
    Component::Component() {
        operation_queue = new Operation_Queue();
    }

    // Delete the operation queue
    Component::~Component() {
        if (operation_queue)
            delete operation_queue;
        for (auto publisher : publishers)
            delete publisher;
        for (auto subscriber : subscribers)
            delete subscriber;
        for (auto client : clients)
            delete client;
        for (auto server : servers)
            delete server;
    }

    // Get Operation Queue
    Operation_Queue *Component::get_operation_queue() {
        return operation_queue;
    }

    // Find timer by name
    Timer *Component::timer(std::string timer_name) {
        for (auto timer : timers)
            if (timer->get_name().compare(timer_name) == 0)
                return timer;
        return NULL;
    }

    // Find publisher by name
    Publisher *Component::publisher(std::string publisher_name) {
        for (auto publisher : publishers)
            if (publisher->get_name().compare(publisher_name) == 0)
                return publisher;
        return NULL;
    }

    // Find subscriber by name
    Subscriber *Component::subscriber(std::string subscriber_name) {
        for (auto subscriber : subscribers)
            if (subscriber->get_name().compare(subscriber_name) == 0)
                return subscriber;
        return NULL;
    }

    // Find client by name
    Client *Component::client(std::string client_name) {
        for (auto client : clients)
            if (client->get_name().compare(client_name) == 0)
                return client;
        return NULL;
    }

    // Find server by name
    Server *Component::server(std::string server_name) {
        for (auto server : servers)
            if (server->get_name().compare(server_name) == 0)
                return server;
        return NULL;
    }

    // Add a new timer
    void Component::add_timer(Timer *new_timer) {
        timers.push_back(new_timer);
    }

    // Add a new publisher
    void Component::add_publisher(Publisher *new_publisher) {
        publishers.push_back(new_publisher);
    }

    // Add a new subscriber
    void Component::add_subscriber(Subscriber *new_subscriber) {
        subscribers.push_back(new_subscriber);
    }

    // Add a new client
    void Component::add_client(Client *new_client) {
        clients.push_back(new_client);
    }

    // Add a new server
    void Component::add_server(Server *new_server) {
        servers.push_back(new_server);
    }

    // Configure publisher bindings
    void Component::configure_publishers(std::map<std::string, std::vector<zcm_endpoint>> publisher_endpoints) {
        for (auto pub_map : publisher_endpoints) {
            Publisher *pub_ptr = publisher(pub_map.first);
            if (pub_ptr)
                pub_ptr->bind(pub_map.second);
            else
                std::cout << "ERROR::Unable to find publisher \"" << pub_map.first << "\"" << std::endl;
        }
    }

    // Configure subscriber connections
    void Component::configure_subscribers(std::map<std::string, std::vector<std::string>> subscriber_endpoints) {
        for (auto sub_map : subscriber_endpoints) {
            Subscriber *sub_ptr = subscriber(sub_map.first);
            if (sub_ptr)
                sub_ptr->connect(sub_map.second);
            else
                std::cout << "ERROR::Unable to find subscriber \"" << sub_map.first << "\"" << std::endl;
        }
    }

    // Configure client connections
    void Component::configure_clients(std::map<std::string, std::vector<std::string>> client_endpoints) {
        for (auto client_map : client_endpoints) {
            Client *client_ptr = client(client_map.first);
            if (client_ptr)
                client_ptr->connect(client_map.second);
            else
                std::cout << "ERROR::Unable to find client \"" << client_map.first << "\"" << std::endl;
        }
    }

    // Configure server bindings
    void Component::configure_servers(std::map<std::string, std::vector<std::string>> server_endpoints) {
        for (auto server_map : server_endpoints) {
            Server *server_ptr = server(server_map.first);
            if (server_ptr)
                server_ptr->bind(server_map.second);
            else
                std::cout << "ERROR::Unable to find server \"" << server_map.first << "\"" << std::endl;
        }
    }

    // Register a functionality
    void Component::register_functionality(std::string operation_name,
                                           std::function<void()> operation_function) {
        functionality[operation_name] = operation_function;
    }

    // Spawn the component executor thread
    std::thread *Component::spawn() {
        executor_thread = operation_queue->spawn();
        for (auto timer : timers)
            timer->start();
        for (auto subscriber : subscribers)
            subscriber->start();
        for (auto server : servers)
            server->start();
        return executor_thread;
    }

}
