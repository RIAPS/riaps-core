//
// Created by istvan on 10/4/17.
//

#ifndef RIAPS_CORE_R_PUBLISHERPORTBASE_H
#define RIAPS_CORE_R_PUBLISHERPORTBASE_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/ports/r_senderport.h>

namespace riaps{
    namespace ports{
        class PublisherPortBase : public PortBase, public SenderPort {
        public:
            PublisherPortBase(const ComponentPortConfig* config, const ComponentBase* parent);
            std::string endpoint();

            virtual const ComponentPortPub* GetConfig() const;
            virtual ~PublisherPortBase() = default;

        protected:
            int port_;
            std::string host_;
            virtual void InitSocket();
        };
    }
}

#endif //RIAPS_CORE_R_PUBLISHERPORTBASE_H
