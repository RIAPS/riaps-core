//
// Created by istvan on 10/4/17.
//

#ifndef RIAPS_CORE_R_PUBLISHERPORTBASE_H
#define RIAPS_CORE_R_PUBLISHERPORTBASE_H

#include <const/r_const.h>
#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/ports/r_senderport.h>

namespace riaps{
    namespace ports{
        class PublisherPortBase : public PortBase, public SenderPort {
        public:
            PublisherPortBase(const ComponentPortConfig* config, const ComponentBase* parent);

            /**
             * @return The ZMQ address of the publisher port.
             */
            std::string endpoint();

            /**
             * @return Configuration parameters of the puvlisher port.
             */
            virtual const ComponentPortPub* GetConfig() const;
            virtual ~PublisherPortBase() = default;

        protected:
            int port_;
            std::string host_;
            std::shared_ptr<zactor_t> auth_;
            virtual void InitSocket();
        };
    }
}

#endif //RIAPS_CORE_R_PUBLISHERPORTBASE_H
