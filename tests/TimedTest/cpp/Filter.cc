#include <Filter.h>

namespace timedtest {
    namespace components {

        Filter::Filter(_component_conf &config, riaps::Actor &actor) :
                FilterBase(config, actor) {
        }

        void Filter::OnReady(const DummyT::Reader &message,
                             riaps::ports::PortBase *port) {
            if (port->GetPortBaseConfig()->isTimed) {
                _logger->info("{}, sendTs: {}.{}, recvTs: {}.{}", __FUNCTION__,
                                 port->AsRecvPort()->GetLastSendTimestamp().tv_sec,
                                 port->AsRecvPort()->GetLastSendTimestamp().tv_nsec,
                                 port->AsRecvPort()->GetLastRecvTimestamp().tv_sec,
                                 port->AsRecvPort()->GetLastRecvTimestamp().tv_nsec);
            } else {
                _logger->info("{}", __FUNCTION__);
            }
            _logger->info("{}", message.getMsg().cStr());

            capnp::MallocMessageBuilder builder;
            auto msg = builder.initRoot<DummyT>();
            msg.setMsg("FromFilter");
            if (SendQuery(builder, msg)){
                DummyT::Reader reader;
                RecvQuery(reader);
            }
        }


        void Filter::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                    capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {

        }

        Filter::~Filter() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
    auto result = new timedtest::components::Filter(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
