//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_subscriberport.h"

namespace riaps{

    SubscriberPort::SubscriberPort() {

    }

    std::unique_ptr<SubscriberPort> SubscriberPort::InitFromServiceDetails(service_details& target_service) {
        std::unique_ptr<SubscriberPort> result(new SubscriberPort());

        std::string targetaddress = "tcp://" + target_service.ip_address + ":" + target_service.port;

        std::cout << "Subscriber connects to: " << targetaddress << std::endl;
        result->port_socket = zsock_new_sub(targetaddress.c_str(), "");

        if (result->port_socket!=NULL) {
            std::cout << "Subscriber connected" << std::endl;
        }
        else {
            std::cout << "Subscriber couldn't connect" << std::endl;
        }

        return std::move(result);
    }

    void SubscriberPort::GetRemoteServiceAsync(subscriber_conf &config, std::string asyncendpoint) {
        get_servicebyname_async(config.remoteservice_name, asyncendpoint);
    }

    service_details SubscriberPort::GetRemoteService(subscriber_conf &config) {
        // Get the target service setails
        bool service_available = false;
        service_details target_service;
        while (!service_available) {

            std::vector<service_details> results;
            get_servicebyname(config.remoteservice_name, results);

            // Wait if the service is not available
            if (!results.empty()){
                target_service = results.front();
                service_available = true;
            } else{
                zclock_sleep(SERVICE_POLLING_INTERVAL);
            }
        }

        return target_service;
    }

    SubscriberPort::~SubscriberPort() {

    }

}