//
// Created by istvan on 9/30/16.
//

#include "componentmodel/r_requestport.h"


namespace riaps {
    namespace ports {

        RequestPort::RequestPort(const _component_port_req_j &config, const ComponentBase *component)
                : PortBase(PortTypes::Request, (component_port_config*)(&config)),
                    _parent_component(component) {

        }

        void RequestPort::Init() {

            _component_port_req_j* current_config = (_component_port_req_j*)_config;

            auto results =
                    subscribe_to_service(_parent_component->GetActor()->GetApplicationName(),
                                         _parent_component->GetConfig().component_name,
                                         _parent_component->GetActor()->GetActorName(),
                                         Kind::REQ,
                                         (current_config->isLocal?Scope::LOCAL:Scope::GLOBAL),
                                         _config->portName, // Subscriber name
                                         current_config->messageType);

            for (auto result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool RequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(_port_socket, rep_endpoint.c_str());

            if (rc != 0) {
                std::cout << "Request '" + _config->portName + "' couldn't connect to " + rep_endpoint
                          << std::endl;
                return false;
            }

            std::cout << "Request port connected to: " << rep_endpoint << std::endl;
            return true;
        }

        // Before sending the publisher sets up the message type
        void RequestPort::Send(zmsg_t *msg) const {
            zmsg_pushstr(msg, ((_component_port_req_j*)_config)->messageType.c_str());

            int rc = zmsg_send(&msg, _port_socket);
            assert(rc == 0);
        }
    }
}

//namespace riaps {
//
//    RequestPort::RequestPort(request_conf& config) {
//        _config = config;
//
//
//        port_socket = zsock_new(ZMQ_REQ);
//    }
//
//    service_details RequestPort::GetRemoteService(request_conf& config) {
//        // Get the target service setails
//        bool service_available = false;
//        service_details target_service;
//        while (!service_available) {
//
//            std::vector<service_details> results;
//            get_servicebyname(config.remoteservice_name, results);
//
//            // Wait if the service is not available
//            if (!results.empty()) {
//                target_service = results.front();
//                service_available = true;
//            } else {
//                zclock_sleep(SERVICE_POLLING_INTERVAL);
//            }
//        }
//
//        return target_service;
//    }
//
//    // TODO: possible memory leak, return something else
//    // TODO: destroy socket... Maybe replacing REQ/REP with REQ/ROUTER???
//    zmsg_t* RequestPort::SendMessage(zmsg_t **msg) {
//        //std::cout << "Request port sends message " << std::endl;
//        service_details target_service = GetRemoteService(_config);
//        std::string targetaddress = "tcp://" + target_service.ip_address + ":" + target_service.port;
//        zsock_connect(port_socket, targetaddress.c_str());
//
//
//        zmsg_send(msg, port_socket);
//        //std::cout << "Request port waits for results" << std::endl;
//        zmsg_t* result = zmsg_recv(port_socket);
//        //std::cout << "Results arrived from response port" << std::endl;
//
//        zsock_disconnect(port_socket, targetaddress.c_str());
//
//        return result;
//    }
//
//    RequestPort::~RequestPort() {}
//}