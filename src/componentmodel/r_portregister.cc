//
// Created by istvan on 9/22/17.
//

#include <componentmodel/r_portregister.h>

namespace riaps{
    PortRegister::PortRegister() {
    }

    const ports::PublisherPort* PortRegister::InitPublisherPort(const _component_port_pub& config) {
        auto result = new ports::PublisherPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    PortRegister::~PortRegister() {

    }
}