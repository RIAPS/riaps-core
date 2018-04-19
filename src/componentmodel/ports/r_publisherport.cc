//
// Created by parallels on 9/6/16.
//

#include <componentmodel/ports/r_publisherport.h>
#include <framework/rfw_network_interfaces.h>
#include <componentmodel/r_discoverdapi.h>


namespace riaps{

    namespace ports {



        PublisherPort::PublisherPort(const component_port_pub &config, const ComponentBase* parentComponent)
            : PublisherPortBase((component_port_config*)&config, parentComponent)

        {
            InitSocket();
            if (!registerService(riaps::Actor::GetRunningActor()->getApplicationName(),
                                 riaps::Actor::GetRunningActor()->getActorName(),
                                 config.messageType,
                                 m_host,
                                 m_port,
                                 riaps::discovery::Kind::PUB,
                                 (config.isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                 {})) {
                throw std::runtime_error("Publisher port couldn't be registered.");
            }

        }





        PublisherPort* PublisherPort::AsPublishPort() {
            return this;
        }



//        bool PublisherPort::Send(std::string& message) const{
//            zmsg_t* zmsg = zmsg_new();
//            zmsg_addstr(zmsg, message.c_str());
//
//            return Send(&zmsg);
//        }
//
//        bool PublisherPort::Send(std::vector<std::string>& fields) const {
//            zmsg_t* zmsg = zmsg_new();
//
//            for (auto it = fields.begin(); it!=fields.end(); it++){
//                zmsg_addstr(zmsg, it->c_str());
//            }
//
//            return Send(&zmsg);
//        }

        PublisherPort::~PublisherPort() {
            // std::cout << "Publisherport " << GetConfig()->portName << " is stopping" <<std::endl;
        }
    }
}