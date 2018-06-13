//
// Created by parallels on 9/29/16.
//

#include <framework/rfw_network_interfaces.h>
#include <componentmodel/ports/r_answerport.h>

namespace riaps{
    namespace ports{

        // TODO: Do not thrwo exception from the constructor
        AnswerPort::AnswerPort(const component_port_ans &config, const ComponentBase *parent_component) :
            PortBase(PortTypes::Answer, (component_port_config*)&config, parent_component),
            SenderPort(this)
        {
            m_port_socket = zsock_new(ZMQ_ROUTER);
            m_host = riaps::framework::Network::GetIPAddress();


            if (m_host == "") {
                throw std::runtime_error("Response cannot be initiated. Cannot find  available network interface.");
            }

            std::string ansEndpoint = "tcp://" + m_host + ":!";
            m_port = zsock_bind(m_port_socket, "%s", ansEndpoint.c_str());


            if (m_port == -1) {
                throw std::runtime_error("Couldn't bind response port.");
            }

            m_logger->info("Answerport is created on: {}:{}", m_host, m_port);


            if (!registerService(riaps::Actor::GetRunningActor()->getApplicationName(),
                                 riaps::Actor::GetRunningActor()->getActorName(),
                                  config.messageType,
                                  m_host,
                                  m_port,
                                  riaps::discovery::Kind::ANS,
                                  (config.isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                  {})) {
                throw std::runtime_error("Answerport couldn't be registered.");
            }
        }

        const component_port_ans* AnswerPort::GetConfig() const{
            return (component_port_ans*)GetPortBaseConfig();
        }

        bool AnswerPort::SendAnswer(capnp::MallocMessageBuilder& builder, std::shared_ptr<MessageParams> params) {
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


//        bool ResponsePort::Send(zmsg_t** msg) const {
//            //zmsg_pushstr(*msg, GetConfig()->rep_type.c_str());
//
//            int rc = zmsg_send(msg, _port_socket);
//            return rc == 0;
//        }

        AnswerPort* AnswerPort::AsAnswerPort() {
            return this;
        }

        AnswerPort::~AnswerPort() noexcept {

        }

    }
}

