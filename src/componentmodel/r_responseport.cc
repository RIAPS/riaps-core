//
// Created by parallels on 9/29/16.
//

#include "componentmodel/r_responseport.h"

namespace riaps{
    ResponsePort::ResponsePort(response_conf &config) {
        configuration = config;
        port_socket = zsock_new(ZMQ_REP);

        std::string endpointstr = "tcp://*:";

        if (config.port == 0) {
            endpointstr += "!";
            port = zsock_bind(port_socket, endpointstr.c_str());
        } else {
            endpointstr += std::to_string(config.port);
            port = zsock_bind(port_socket, endpointstr.c_str());
        }

        endpoint = std::string(zsock_endpoint(port_socket));

        std::cout << "Server (ZMQ_REP) is created on port: " << port << std::endl;

        std::string ifaddress = GetInterfaceAddress(config.network_iface);

        // TODO: error handling
        // NOTE: Should be separated form construct
        if (ifaddress!="") {

            std::cout << "Registering ZMQ_REP" << std::endl;

            // TODO: Add tags
            register_service(config.servicename, config.servicename, ifaddress, std::to_string(port), {});
        }
        else{
            std::cout << "iface doesn't exist: " << config.network_iface << std::endl;
        }

    }

    response_conf ResponsePort::GetConfig() {
        return configuration;
    }

    std::string ResponsePort::GetEndpoint() {
        if (port_socket) {
            return std::string(zsock_endpoint(port_socket));
        }
        return "";
    }

    void ResponsePort::SendMessage(zmsg_t **msg) {
        int rc = zmsg_send(msg, port_socket);
        assert(rc==0);
    }

    ResponsePort::~ResponsePort() {
        deregister_service(configuration.servicename);
    }
}