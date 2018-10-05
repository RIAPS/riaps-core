#ifndef RIAPS_CORE_CBASE_H
#define RIAPS_CORE_CBASE_H

#include <componentmodel/r_componentbase.h>
#include <messages/consensus.capnp.h>

#define PORT_TIMER_CLOCK  "clock"
#define GROUP_TYPE_GROUP1 "group1"

namespace dc {
    namespace components {

        class ConsensusBase : public riaps::ComponentBase {

        public:
            ConsensusBase(_component_conf &config, riaps::Actor &actor);
            virtual ~ConsensusBase();

        protected:

            virtual void OnClock(riaps::ports::PortBase *port)=0;


        private:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> params);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);


        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H
