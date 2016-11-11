//
// Created by parallels on 9/29/16.
//

#include "componentmodel/r_responseport.h"

namespace riaps{


    std::unique_ptr<ResponsePort> ResponsePort::InitFromConfig(response_conf& config) {
        std::unique_ptr<ResponsePort> result(new ResponsePort());

        result->configuration = config;
        result->port_socket = zsock_new(ZMQ_REP);

        if (config.port == 0) {
            result->port = zsock_bind(result->port_socket, "tcp://*:!");
        } else {
            char tmp[256];
            sprintf(tmp, "tcp://*:%d", config.port);
            result->port = zsock_bind(result->port_socket, tmp);
        }

        result->endpoint = std::string(zsock_endpoint(result->port_socket));

        std::cout << "ResponsePort is created on port: " << result->port << std::endl;

        std::string ifaddress = result->GetInterfaceAddress(config.network_iface);

        // TODO: error handling
        // NOTE: Should be separated form construct
        if (ifaddress!="") {

            std::cout << "Registering response port" << std::endl;

            // TODO: Add tags
            //register_service(config.servicename, config.servicename, ifaddress, std::to_string(result->port), {});
        }

        return std::move(result);
    }


    ResponsePort::ResponsePort(){

    }

    //ResponsePort::ResponsePort(response_conf &config) {
//
    //}

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