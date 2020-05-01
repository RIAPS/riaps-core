//
// Created by istvan on 10/13/17.
//

#ifndef RIAPS_CORE_R_SUBPORTBASE_H
#define RIAPS_CORE_R_SUBPORTBASE_H

//#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_portbase.h>
//

#include <string>
#include <memory>

#include "r_recvport.h"

namespace riaps{
    namespace ports{
        class SubscriberPortBase : public PortBase, public RecvPort{
        public:
            /**
             * Initializes and stores a ZMQ SUB socket.
             * @param config Port metadata.
             * @param parent Parent component.
             */
            SubscriberPortBase(const ComponentPortConfig* config, const ComponentBase* parent);

            /**
             * @return Pointer to the configuration structure.
             */
            virtual const ComponentPortSub* GetConfig() const;

            /**
             * Connects to a publisher port.
             * @param endpoint IP address of the publisher port
             * @return False if the connection couldn't be made.
             */
            bool ConnectToPublihser(const std::string& endpoint);

            virtual void Init() = 0;

            ~SubscriberPortBase() override;

        protected:
            /**
             * List of ip addresses where the port is connected to
             */
            std::vector<std::string> endpoints_;

            /**
             * Creates the ZMQ SUB socket and sets the filter to "" (no filtering).
             */
            virtual void InitSocket();
        };
    }
}

#endif //RIAPS_CORE_R_SUBPORTBASE_H
