#include <framework/rfw_network_interfaces.h>
#include <componentmodel/ports/r_answerport.h>

using namespace std;
using namespace riaps::discovery;

namespace riaps{
    namespace ports{

        AnswerPort::AnswerPort(const ComponentPortAns &config, const ComponentBase *parent_component) :
            PortBase(PortTypes::Answer, (ComponentPortConfig*)&config, parent_component),
            SenderPort(this)
        {
            port_socket_ = zsock_new(ZMQ_ROUTER);
            auto iface = riaps::framework::Network::GetConfiguredIface();
            host_ = riaps::framework::Network::GetIPAddress(iface);


            if (host_ == "") {
                logger()->error("Response cannot be initiated. Cannot find  available network interface.");
            }

            // The port is NOT local AND encrypted
            //if (!GetConfig()->is_local && has_security()) {
            if (has_security()) {
                if (port_certificate_ != nullptr) {
                    zcert_apply (port_certificate_.get(), port_socket_);
                    zsock_set_curve_server (port_socket_, 1);
                } else {
                    logger()->error("Port certificate is null, cannot create port: {}", port_name());
                    return;
                }
            }

            string end_point = fmt::format("tcp://{}:!", host_);
            port_ = zsock_bind(port_socket_, "%s", end_point.c_str());


            if (port_ == -1) {
                logger()->error("Couldn't bind response port.");
            }

            logger()->info("Answerport is created on: {}:{}", host_, port_);

            if (!Disco::RegisterService(
                    parent_component->actor()->application_name(),
                    parent_component->actor()->actor_name(),
                    config.message_type,
                    host_,
                    port_,
                    riaps::discovery::Kind::ANS,
                    (config.is_local ? riaps::discovery::Scope::LOCAL : riaps::discovery::Scope::GLOBAL),
                    {})) {
                logger()->error("Answerport couldn't be registered.");
            }
        }

        const ComponentPortAns* AnswerPort::GetConfig() const{
            return (ComponentPortAns*) config();
        }
        
        PortError AnswerPort::SendAnswer(capnp::MallocMessageBuilder& builder, std::shared_ptr<MessageParams> params) {
            zmsg_t* msg = zmsg_new();

            // Message to query port, first frame must be the SocketId
            zmsg_pushstr(msg, params->GetOriginId().c_str());

            // Original requestId (added in qry-ans)
            zmsg_addstr(msg, params->GetRequestId().c_str());

            zframe_t* userFrame;
            userFrame << builder;

            zmsg_add(msg, userFrame);

            // Has timestamp
            int64_t ztimeStamp;
            if (params->HasTimestamp()) {
                ztimeStamp = zclock_time();
                zmsg_addmem(msg, &ztimeStamp, sizeof(ztimeStamp));
            } else {
                zmsg_addmem(msg, nullptr, 0);
            }
            return Send(&msg);
        }

        AnswerPort::~AnswerPort() noexcept {

        }

    }
}

