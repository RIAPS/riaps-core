//
// Created by parallels on 9/6/16.
//

#include "componentmodel/r_subscriberport.h"

namespace riaps{

    // TODO: ? timeout needed ? (when the subscriber gives up)
//    SubscriberPort::SubscriberPort(subscriber_conf& config) {
//
//        // Get the target service setails
//        bool service_available = false;
//        service_details target_service;
//        while (!service_available) {
//
//            std::vector<service_details> results;
//            get_servicebyname(config.remoteservice_name, results);
//
//            // Wait if the service is not available
//            if (!results.empty()){
//                target_service = results.front();
//                service_available = true;
//            } else{
//                zclock_sleep(SERVICE_POLLING_INTERVAL);
//            }
//        }
//
//        std::string targetaddress = "tcp://" + target_service.ip_address + ":" + target_service.port;
//        port_socket = zsock_new_sub(targetaddress.c_str(), "");
//
//        if (port_socket!=NULL) {
//            std::cout << "Subscriber connected" << std::endl;
//        }
//        else {
//            std::cout << "Subscriber couldn't connect" << std::endl;
//        }
//
//    }

    SubscriberPort::SubscriberPort(subscriber_conf &config) {
        // Just send the async request
        //get_servicebyname_async(config.remoteservice_name, "ipc://lofasz");
    }

    SubscriberPort::~SubscriberPort() {

    }

}