//
// Created by parallels on 9/7/16.
//

#include "componentmodel/r_portbase.h"

namespace riaps {
    PortBase::PortBase() {
        port_socket = NULL;
    }

    const zsock_t * PortBase::GetSocket() {
        return port_socket;
    }

    std::string PortBase::GetInterfaceAddress(std::string ifacename){
        ziflist_t *iflist = ziflist_new ();
        assert (iflist);
        size_t items = ziflist_size (iflist);

        const char *name = ziflist_first (iflist);
        std::string result = "";

        while (name && result == "") {
            std::string namestr(name);
            if (namestr == ifacename){
                result = ziflist_address (iflist);
            }
            name = ziflist_next (iflist);
        }

        ziflist_reload (iflist);
        assert (items == ziflist_size (iflist)); ziflist_destroy (&iflist);

        return result;
    }

    PortBase::~PortBase() {
        std::cout << "Destroy called " << std::endl;
        if (port_socket) {
            std::cout << "Destroy portbase socket : " << port_socket <<std::endl;
            zsock_destroy(&port_socket);
        }
    }
}