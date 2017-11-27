

#ifndef RIAPSAPP_CLIENT_H
#define RIAPSAPP_CLIENT_H

#include "componentmodel/r_componentbase.h"
#include "messages/activereplica.capnp.h"
#include "GroupTypes.h"

#define PORT_SUB_ESTIMATE "estimate"
#define PORT_TIMER_WAKEUP "wakeup"

namespace activereplica{
    namespace components{
        class ClientBase : public riaps::ComponentBase {

        public:

            ClientBase(_component_conf& config, riaps::Actor& actor);

            virtual void OnEstimate(messages::Estimate::Reader& message,
                                    riaps::ports::PortBase* port)=0;

            virtual void OnWakeup(riaps::ports::PortBase* port)=0;

            virtual ~ClientBase();

        protected:
            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase*   port,
                                         std::shared_ptr<riaps::AsyncInfo> asyncInfo = nullptr);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);

        };
    }
}


#endif //RIAPSAPP_CLIENT_H
