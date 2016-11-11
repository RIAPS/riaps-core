//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_publisherport.h>

namespace riaps{

    // TODO: Pass scope
    PublisherPort::PublisherPort(_component_port_pub_j& config, std::string app_name) {
        _configuration = config;
        port_socket = zsock_new(ZMQ_PUB);


        if (config.port == 0) {
            _port = zsock_bind(port_socket, "tcp://*:!");
        } else {
            char tmp[256];
            sprintf(tmp, "tcp://*:%d", config.port);
            _port = zsock_bind(port_socket, tmp);
        }

        _endpoint = std::string(zsock_endpoint(port_socket));

        std::cout << "Publisher is created on port: " << _port << std::endl;

        std::string ifaddress = GetInterfaceAddress();

        if (ifaddress!="") {
            register_service(app_name, config.message_type, ifaddress, config.port, Kind::PUB, Scope::GLOBAL, {});
        }
        else{
            throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
        }

    }

    _component_port_pub_j PublisherPort::GetConfig() {
        return _configuration;
    }

    std::string PublisherPort::GetEndpoint() {
        if (port_socket) {
            return std::string(zsock_endpoint(port_socket));
        }
        return "";
    }

    void PublisherPort::PublishMessage(zmsg_t **msg) {
        int rc = zmsg_send(msg, port_socket);
        assert(rc==0);
    }

    PublisherPort::~PublisherPort() {
        deregister_service(_configuration.message_type);
    }
}