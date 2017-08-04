//
// Created by istvan on 4/13/17.
//

#include <base/CollectorBase.h>

namespace testing {
    namespace components {

        CollectorBase::CollectorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config, actor) {

        }

        void CollectorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                                  riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_SUB_GETTEMPERATURE) {
                auto estimate = capnpreader->getRoot<messages::SensorValue>();
                OnGetTemperature(estimate, port);
            }
        }

		void CollectorBase::DispatchInsideMessage(zmsg_t*, riaps::ports::PortBase*)
		{
		}

        CollectorBase::~CollectorBase() {

        }
    }
}