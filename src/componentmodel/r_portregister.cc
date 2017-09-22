//
// Created by istvan on 9/22/17.
//

#include <componentmodel/r_portregister.h>

namespace riaps{
    const ports::PublisherPort* PortRegister::InitPublisherPort(const _component_port_pub& config) {
        auto result = new ports::PublisherPort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::SubscriberPort* PortRegister::InitSubscriberPort(const _component_port_sub& config) {
        std::unique_ptr<ports::SubscriberPort> newport(new ports::SubscriberPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::ResponsePort* PortRegister::InitResponsePort(const _component_port_rep & config) {
        auto result = new ports::ResponsePort(config, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::RequestPort*   PortRegister::InitRequestPort(const _component_port_req& config){
        std::unique_ptr<ports::RequestPort> newport(new ports::RequestPort(config, this));
        auto result = newport.get();
        newport->Init();
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::InsidePort* PortRegister::InitInsiderPort(const _component_port_ins& config) {
        auto result = new ports::InsidePort(config, riaps::ports::InsidePortMode::BIND, this);
        std::unique_ptr<ports::PortBase> newport(result);
        _ports[config.portName] = std::move(newport);
        return result;
    }

    const ports::PeriodicTimer* PortRegister::InitTimerPort(const _component_port_tim& config) {
        std::string timerchannel = GetTimerChannel();
        std::unique_ptr<ports::PeriodicTimer> newtimer(new ports::PeriodicTimer(timerchannel, config));
        newtimer->start();

        auto result = newtimer.get();

        _ports[config.portName] = std::move(newtimer);
        return result;
    }
}