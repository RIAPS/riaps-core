//
// Created by parallels on 9/7/16.
//

#include "componentmodel/r_portbase.h"

namespace riaps {

    namespace ports {

        PortBase::PortBase(PortTypes portType) {
            _port_type = portType;
        }

        void PortBase::Send(zmsg_t *msg) const {
            throw std::runtime_error("Send not implemented on this kind of port.");
        }

        const zsock_t *PortBase::GetSocket() const {
            return _port_socket;
        }

        std::string PortBase::GetInterfaceAddress() {
            ziflist_t *iflist = ziflist_new();
            assert (iflist);
            //size_t items = ziflist_size(iflist);

            const char *name = ziflist_first(iflist);
            std::string result = "";

            while (name && result == "") {
                std::string address(ziflist_address(iflist));
                //std::string namestr(name);
                //
                if (address != "127.0.0.1") {
                    result = address;
                    break;
                }
                name = ziflist_next(iflist);

                // break;
            }

            return result;
        }

        const PortTypes& PortBase::GetPortType() const {
            return _port_type;
        }
        /*PortBase::PortBase(const ComponentBase* parentComponent) {
            port_socket = NULL;
            _parentComponent = parentComponent;
        }*/

        /*PortBase::PortBase() {

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

            return result;
        }

    */

        PortBase::~PortBase() {
            std::cout << "Destroy called " << std::endl;
            if (_port_socket) {
                std::cout << "Destroy portbase socket : " << _port_socket << std::endl;
                zsock_destroy(&_port_socket);
            }
        }
    }
}