//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_ANSWERPORT_H
#define RIAPS_FW_R_ANSWERPORT_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>

#include "czmq.h"

#include <string>
#include <memory>

namespace riaps {

    class ComponentBase;

    /*
     * RIAPS Ports.
     */
    namespace ports {

        class AnswerPort : public PortBase, public SenderPort {
        public:
            /**
             * @param config Config parameters.
             * @param parent Component instance which owns the port.
             */
            AnswerPort(const ComponentPortAns &config, const ComponentBase* parent);

            /**
             * Loads and applies the CURVE keys.
             */
            void InitSecurity();

            /**
             * Registers the port in the discovery service
             */
            void RegisterPort();

            /**
             * Binds the socket to local address.
             */
            void Init();

            /**
             * @return Config parameters of the port.
             */
            virtual const ComponentPortAns* GetConfig() const;

            /**
             * Sends a message on the port.
             * @param builder The message as capnp buffer
             * @param params Parameters for the answer port (id in the first ZMQ frame)
             * @return PortError
             */
            //PortError SendAnswer(capnp::MallocMessageBuilder& builder, const std::string& identity);

            std::string host() const;
            int port() const;

            ~AnswerPort() noexcept ;
        protected:
            std::shared_ptr<zactor_t> auth_;
            int                       port_;
            std::string               host_;
            std::string               endpoint_;
        };

    }
}

#endif //RIAPS_FW_R_ANSWERPORT_H
