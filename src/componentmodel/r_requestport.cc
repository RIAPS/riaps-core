//
// Created by istvan on 9/30/16.
//

#include "componentmodel/r_requestport.h"


namespace riaps {

    RequestPort::RequestPort(request_conf& config) {
        _config = config;


        port_socket = zsock_new(ZMQ_REQ);
    }

    service_details RequestPort::GetRemoteService(request_conf& config) {
        // Get the target service setails
        bool service_available = false;
        service_details target_service;
        while (!service_available) {

            std::vector<service_details> results;
            get_servicebyname(config.remoteservice_name, results);

            // Wait if the service is not available
            if (!results.empty()) {
                target_service = results.front();
                service_available = true;
            } else {
                zclock_sleep(SERVICE_POLLING_INTERVAL);
            }
        }

        return target_service;
    }

    // TODO: possible memory leak, return something else
    // TODO: destroy socket... Maybe replacing REQ/REP with REQ/ROUTER???
    zmsg_t* RequestPort::SendMessage(zmsg_t **msg) {
        std::cout << "Request port sends message " << std::endl;
        service_details target_service = GetRemoteService(_config);
        std::string targetaddress = "tcp://" + target_service.ip_address + ":" + target_service.port;
        zsock_connect(port_socket, targetaddress.c_str());


        zmsg_send(msg, port_socket);
        std::cout << "Request port waits for results" << std::endl;
        zmsg_t* result = zmsg_recv(port_socket);
        std::cout << "Results arrived from response port" << std::endl;

        zsock_disconnect(port_socket, targetaddress.c_str());

        return result;
    }

    RequestPort::~RequestPort() {}
}