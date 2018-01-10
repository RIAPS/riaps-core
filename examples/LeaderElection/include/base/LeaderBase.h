#ifndef RIAPS_CORE_CBASE_H
#define RIAPS_CORE_CBASE_H

#include <componentmodel/r_componentbase.h>
#include <messages/leaderelection.capnp.h>

#define PORT_TIMER_CLOCK "clock"

namespace leaderelection {
    namespace components {

        class LeaderBase : public riaps::ComponentBase {

        public:
            LeaderBase(_component_conf &config, riaps::Actor &actor);
            virtual ~LeaderBase();

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
