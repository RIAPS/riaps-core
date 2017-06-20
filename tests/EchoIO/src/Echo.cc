#include <Echo.h>

namespace echoio{
    namespace components{
        Echo::Echo(_component_conf_j &config, riaps::Actor &actor) : EchoBase(config, actor){

        }

        void Echo::OnClock(riaps::ports::PortBase *port) {
            std::cout << "Echo::OnClock()" << std::endl;
        }

        void Echo::OnEcho(const messages::EchoReq::Reader &message, riaps::ports::PortBase *port) {
            auto msgValue = message.getValue();
            std::string reversed;

            reverseString(msgValue, reversed);

            capnp::MallocMessageBuilder builder;
            auto echoRep = builder.initRoot<echoio::messages::EchoRep>();

            echoRep.setMsg(reversed.c_str());
            SendEcho(builder, echoRep);
        }

        void OnOneShotTimer(const std::string& timerid){

        }

        void Echo::reverseString(const std::string &original, std::string &reversed) {
            reversed = original;
            std::reverse(reversed.begin(), reversed.end());
        }

        Echo::~Echo() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new echoio::components::Echo(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}