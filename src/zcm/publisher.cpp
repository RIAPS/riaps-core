/** @file    publisher.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Publisher class
 */

#include "../../include/zcm/publisher.hpp"



namespace zcm {

    // Construct a simple publisher
    Publisher::Publisher(std::string name,
                         zmq::context_t *actor_context) :
            servicebase(name),
            context(actor_context)
            {}

    // Construct a publisher with known endpoints
    // TODO: endpoints by reference
    Publisher::Publisher(std::string name, zmq::context_t *actor_context,
                         std::vector<zcm_endpoint> endpoints) :
            servicebase(name),
            endpoints(endpoints) {
        context = actor_context;
        publisher_socket = new zmq::socket_t(*context, ZMQ_PUB);
        for (auto endpoint : endpoints) {
            try {

                std::string zmq_endpoint = endpoint.address + ":" + endpoint.port;

                // TODO: put tags in the config file
                std::vector<std::string> tags;

                // TODO: check for success
                publisher_socket->bind(zmq_endpoint);

                register_service(name, name, endpoint.address, endpoint.port, tags);

            } catch (zmq::error_t error) {
                std::cout << "Unable to bind publisher " <<
                          name << " to " << endpoint.address << " on port " << endpoint.port << std::endl;
            }
        }
    }

    // Destroy the publisher
    Publisher::~Publisher() {
        publisher_socket->close();
        delete context;
        delete publisher_socket;
    }

    // Bind a publisher to a new set of endpoints
    // TODO: Reregister in consul
    void Publisher::bind(std::vector<zcm_endpoint> new_endpoints) {
        endpoints = new_endpoints;
        publisher_socket = new zmq::socket_t(*context, ZMQ_PUB);
        for (auto endpoint : endpoints) {
            try {
                std::string zmq_endpoint = endpoint.address + ":" + endpoint.port;
                // TODO: put tags in the config file
                std::vector<std::string> tags;
                publisher_socket->bind(zmq_endpoint);

                register_service(name, name, endpoint.address, endpoint.port, tags);

            } catch (zmq::error_t error) {
                std::cout << "Unable to bind publisher " <<
                          name << " to " << endpoint.address << std::endl;
            }
        }
    }



    // Add a new endpoint to the publisher
    void Publisher::add_connection(std::string new_connection) {
        try {
            publisher_socket->bind(new_connection);
        } catch (zmq::error_t error) {
            std::cout << "ERROR::add_connection failed trying to bind to address: "
                      << new_connection << std::endl;
        }
    }

    // Publish a new message
    void Publisher::send(std::string message) {
        zmq::message_t message_struct(message.length());
        memcpy(message_struct.data(), message.c_str(), message.length());
        publisher_socket->send(message_struct);
    }

}
