//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_publisherport.h>

namespace riaps{
    PublisherPort::PublisherPort(publisher_conf& config) {
        configuration = config;
        publisher_socket = zsock_new(ZMQ_PUB);
        port = zsock_bind(publisher_socket, "tcp://*:!");
        endpoint = std::string(zsock_endpoint(publisher_socket));

        std::cout << "Publisher is created on port: " << port << std::endl;

        std::string ifaddress = GetInterfaceAddress(config.network_iface);

        // TODO: error handling
        // NOTE: Should be separated form construct
        if (ifaddress!="") {

            std::cout << "Registering publisher" << std::endl;

            // TODO: Add tags
            register_service(config.servicename, config.servicename, ifaddress, std::to_string(port), {});
        }

    }

    publisher_conf PublisherPort::GetConfig() {
        return configuration;
    }

    std::string PublisherPort::GetEndpoint() {
        if (publisher_socket) {
            return std::string(zsock_endpoint(publisher_socket));
        }
        return "";
    }

    PublisherPort::~PublisherPort() {
        deregister_service(configuration.servicename);
        zsock_destroy(&publisher_socket);
    }
}