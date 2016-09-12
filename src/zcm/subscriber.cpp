/** @file    subscriber.cpp 
 *  @author  Pranav Srinivas Kumar, Istvan Madari
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Subscriber class
 */

#include "../../include/zcm/subscriber.hpp"


// TODO: Remove endpoint concept, replace it with search service by name
namespace zcm {

    Subscriber::Subscriber(std::string name,
                           unsigned int priority,
                           zmq::context_t *actor_context,
                           std::string filter,
                           std::vector<std::string> publishernames,
                           std::function<void()> operation_function,
                           Operation_Queue *operation_queue_ptr) :
            name(name),
            priority(priority),
            filter(filter),
            publishernames(publishernames),
            operation_function(operation_function),
            operation_queue_ptr(operation_queue_ptr) {
        context = actor_context;
        subscriber_socket = new zmq::socket_t(*context, ZMQ_SUB);

        std::cout << "Init Seubscirber" << std::endl;

        for (auto publishername : publishernames) {

            std::cout<< "Get publishername: " << publishername << std::endl;

            std::string endpoint = get_publisher_address(publishername);

            std::cout << "Endpoint: " << endpoint << std::endl;

            if (endpoint!=""){
                subscriber_socket->connect(endpoint);
            }
            else{
                std::cout << "Publisher not found" << std::endl;
            }
        }

        subscriber_socket->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.length());
    }

    Subscriber::~Subscriber() {
        subscriber_socket->close();
        delete context;
        delete subscriber_socket;
    }

    std::string Subscriber::get_publisher_address(std::string publishername){
        // Get the endpoint from consul
        std::vector<service_details> services;
        get_servicebyname(publishername, services);

        // TODO: something more intelligent than size()
        if (services.size()>0){
            service_details first_service = services[0];
            std::string endpoint = first_service.ip_address + ":" + first_service.port;
            return endpoint;
        }
        return "";
    }

    void Subscriber::connect(std::vector<std::string> new_publishernames) {
        publishernames = new_publishernames;
        subscriber_socket = new zmq::socket_t(*context, ZMQ_SUB);
        for (auto publishername : publishernames) {
            std::string endpoint = get_publisher_address(publishername);
            if (endpoint!=""){
                subscriber_socket->connect(endpoint);
            }
            else{
                std::cout << "Publisher not found" << std::endl;
            }
        }
        subscriber_socket->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.length());
    }

    std::string Subscriber::get_name() {
        return name;
    }

    unsigned int Subscriber::get_priority() {
        return priority;
    }

    void Subscriber::add_connection(std::string new_connection) {
        subscriber_socket->connect(new_connection);
    }

    void Subscriber::recv() {
        while (true) {
            zmq::message_t received_message;
            subscriber_socket->recv(&received_message);
            std::string message = std::string(static_cast<char *>(received_message.data()),
                                              received_message.size());
            if (message.length() > 0) {
                func_mutex.lock();
                buffer.push(message);
                Subscriber_Operation *new_operation
                        = new Subscriber_Operation(name, priority, operation_function);
                operation_queue_ptr->enqueue(new_operation);
                func_mutex.unlock();
            }

            //TODO: Remove usleep
            usleep(500);
        }
    }

    void Subscriber::rebind_operation_function(std::function<void()>
                                               new_operation_function) {
        func_mutex.lock();
        operation_function = new_operation_function;
        func_mutex.unlock();
    }

    std::thread Subscriber::spawn() {
        return std::thread(&Subscriber::recv, this);
    }

    void Subscriber::start() {
        std::thread subscriber_thread = spawn();
        subscriber_thread.detach();
    }

    bool Subscriber::is_buffer_empty() {
        if (buffer.empty())
            return true;
        else
            return false;
    }

    std::string Subscriber::message() {
        std::string first_message = "";
        if (!is_buffer_empty()) {
            first_message = buffer.front();
            buffer.pop();
        }
        return first_message;
    }

}
