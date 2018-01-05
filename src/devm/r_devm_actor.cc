//
// Created by istvan on 5/4/17.
//

#include <framework/rfw_network_interfaces.h>
#include <devm/r_devm_actor.h>
#include <devm/r_devm_handlers.h>
#include <messaging/devm.capnp.h>
#include <discoveryd/r_discovery_types.h>
#include <capnp/serialize.h>
#include <capnp/message.h>
#include <czmq.h>
#include <string>
#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include <set>

void devm_zactor (zsock_t *pipe, void *args){

    std::string mac_address = riaps::framework::Network::GetMacAddressStripped();
    std::string host_address = riaps::framework::Network::GetIPAddress();

//    std::unique_ptr<std::map<std::string, actor_details_t*>,
//                    std::function<void(std::map<std::string, actor_details_t*>*)>>
//
//            clients(new std::map<std::string, actor_details_t*>,[](std::map<std::string, actor_details_t*>* ptr)
//    {
//        std::cout << "destroying from a custom deleter...\n";
//        for (auto it = ptr->begin(); it!=ptr->end(); it++){
//            if (it->second == NULL) continue;
//            std::cout << "Delete: " << it->second->port << std::endl;
//            //delete it->second;
//            //it->second = NULL;
//        }
//        delete ptr;
//    });

    std::unique_ptr<std::map<std::string, actor_details_t*>> clients(new std::map<std::string, actor_details_t*>);


    std::string endpoint = DEVMANAGER_SERVICE_IPC + mac_address;
    zsock_t *devmServer = zsock_new_rep(endpoint.c_str());
    assert(devmServer);

    zpoller_t* poller = zpoller_new(pipe, NULL);
    assert(poller);

    zpoller_set_nonstop(poller, true);
    //zpoller_ignore_interrupts (poller);

    zpoller_add(poller, devmServer);

    zsock_signal (pipe, 0);

    bool terminated = false;

    while(!terminated){
        void* which = zpoller_wait(poller, 500);

        if (which == pipe) {
            zmsg_t *msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {

                std::cout << "$TERM arrived in devm" << std::endl;
                terminated = true;
            }
            zstr_free(&command);
            zmsg_destroy(&msg);
        } else if (which == devmServer){
            zmsg_t* zmsg = zmsg_recv(devmServer);

            zframe_t* capnpMsgBody = zmsg_pop(zmsg);
            size_t    size = zframe_size(capnpMsgBody);
            byte*     data = zframe_data(capnpMsgBody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto devmRequest = reader.getRoot<riaps::devm::DevmReq>();

            riaps::devm::DevmHandler::HandleDevmReq(devmRequest, clients.get(), devmServer);

            zmsg_destroy(&zmsg);
            zframe_destroy(&capnpMsgBody);

        }
    }
    std::cout << "devm actor stopped, clean up the ports and the poller" << std::endl;

    riaps::devm::DevmHandler::StopAllDevices();

    for (auto it = clients->begin(); it!=clients->end(); it++){
        delete it->second;
    }

    zpoller_destroy(&poller);
    zsock_destroy(&devmServer);
    zclock_sleep(1000);

    std::cout << "devm was cleaned up" << std::endl;
}