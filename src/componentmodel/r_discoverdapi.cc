#include <groups/r_group.h>
#include <componentmodel/r_discoverdapi.h>
#include <framework/rfw_configuration.h>
#include <spdlog/spdlog.h>

using namespace std;

namespace riaps {
    namespace discovery {
        zsock_t* Disco::RegisterActor(const string &app_name, const string &actor_name) {
            /////
            /// Request
            /////
            capnp::MallocMessageBuilder message;
            riaps::discovery::DiscoReq::Builder dreqBuilder = message.initRoot<riaps::discovery::DiscoReq>();
            riaps::discovery::ActorRegReq::Builder areqBuilder = dreqBuilder.initActorReg();

            areqBuilder.setActorName(actor_name);
            areqBuilder.setAppName(app_name);
            areqBuilder.setVersion("0");
            areqBuilder.setPid(::getpid());

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            string ipcAddress = riaps::framework::Configuration::GetDiscoveryEndpoint();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            /////
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            auto      data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msg_discoreq= reader.getRoot<riaps::discovery::DiscoRep>();

            zsock_t* discovery_port = NULL;

            // Register actor
            if (msg_discoreq.isActorReg()) {
                auto msg_response = msg_discoreq.getActorReg();
                auto status = msg_response.getStatus();
                auto port = msg_response.getPort();


                if (status == riaps::discovery::Status::ERR){
                    std::cout << "Couldn't register actor: " << actor_name << std::endl;
                }
                else {
                    auto discovery_endpoint = "tcp://localhost:" + std::to_string(port);
                    discovery_port = zsock_new_pair(discovery_endpoint.c_str());
                    assert(discovery_port);
                }
            }

            /////
            /// Clean up
            /////
            zsock_disconnect(client, "%s", ipcAddress.c_str());
            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return discovery_port;
        }

        void Disco::DeregisterActor(const std::string &actor_name, const std::string &app_name) {
            auto _logger = spdlog::get(actor_name);
            capnp::MallocMessageBuilder message;
            auto msgDiscoReq   = message.initRoot<riaps::discovery::DiscoReq>();
            auto msgActorUnreg = msgDiscoReq.initActorUnreg();

            msgActorUnreg.setActorName(actor_name);
            msgActorUnreg.setPid(::getpid());
            msgActorUnreg.setAppName(app_name);

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msgReq = zmsg_new();
            zmsg_pushmem(msgReq, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            std::string ipcAddress = riaps::framework::Configuration::GetDiscoveryEndpoint();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);
            zmsg_send(&msgReq, client);

            zmsg_t* msgRep = zmsg_recv(client);

            zframe_t* capnpBody = zmsg_pop(msgRep);
            size_t    size = zframe_size(capnpBody);
            auto      data = zframe_data(capnpBody);

            auto capnpBuffer = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnpBuffer);
            auto msgDiscoRep= reader.getRoot<riaps::discovery::DiscoRep>();
            if (   msgDiscoRep.isActorUnreg()
                && msgDiscoRep.getActorUnreg().getStatus() == riaps::discovery::Status::ERR) {
                _logger->error("Couldn't deregister actor: {} PID: {}",actor_name,::getpid());
            }

            zmsg_destroy(&msgRep);
            zframe_destroy(&capnpBody);
            zsock_destroy(&client);
            zclock_sleep(100);
        }

        bool Disco::RegisterService(const std::string &app_name,
                                    const std::string &actor_name,
                                    const std::string &message_type,
                                    const std::string &ip_address,
                                    const uint16_t &port,
                                    riaps::discovery::Kind kind,
                                    riaps::discovery::Scope scope
        ) {

            bool result = false;

            /////
            /// Request
            /////
            capnp::MallocMessageBuilder message;
            auto dreqBuilder = message.initRoot<riaps::discovery::DiscoReq>();
            auto sreqBuilder = dreqBuilder.initServiceReg();

            auto sreqpath   = sreqBuilder.initPath();
            auto sreqsocket = sreqBuilder.initSocket();

            sreqpath.setAppName(app_name);
            sreqpath.setMsgType(message_type);
            sreqpath.setActorName(actor_name);
            sreqpath.setKind(kind);
            sreqpath.setScope(scope);
            sreqBuilder.setPid(::getpid());

            sreqsocket.setHost(ip_address);
            sreqsocket.setPort(port);

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zsock_t * client = zsock_new_req (riaps::framework::Configuration::GetDiscoveryEndpoint().c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            /////
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            auto      data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msg_discoreq= reader.getRoot<riaps::discovery::DiscoRep>();


            // Register actor
            if (msg_discoreq.isServiceReg()) {
                auto msg_response = msg_discoreq.getServiceReg();
                auto status = msg_response.getStatus();

                // TODO: Check status
                if (status == riaps::discovery::Status::OK){
                    result = true;
                }
            }

            /////
            /// Clean up
            /////

            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return result;
        }

        std::vector<ServiceLookupResult> Disco::SubscribeToService(const std::string &app_name,
                                                                   const std::string &part_name, // instance_name
                                                                   const std::string &actor_name,
                                                                   const std::string &ip_address,
                                                                   riaps::discovery::Kind kind,
                                                                   riaps::discovery::Scope scope,
                                                                   const std::string &port_name,
                                                                   const std::string &msg_type  // port_type
        ){
            std::vector<ServiceLookupResult> result;

            /////
            /// Request
            /////
            capnp::MallocMessageBuilder message;
            auto dreqBuilder = message.initRoot<riaps::discovery::DiscoReq>();
            auto slookupBuilder = dreqBuilder.initServiceLookup();

            auto pathBuilder = slookupBuilder.initPath();
            auto clientBuilder = slookupBuilder.initClient();

            pathBuilder.setAppName(app_name);
            pathBuilder.setMsgType(msg_type);
            pathBuilder.setKind(kind);
            pathBuilder.setScope(scope);

            clientBuilder.setActorName(actor_name);
            clientBuilder.setInstanceName(part_name);
            clientBuilder.setPortName(port_name);
            clientBuilder.setActorHost(ip_address);

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zsock_t * client = zsock_new_req (riaps::framework::Configuration::GetDiscoveryEndpoint().c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            /////
            zmsg_t* msg_response = zmsg_recv(client);

            zframe_t* capnp_msgbody = zmsg_pop(msg_response);
            size_t    size = zframe_size(capnp_msgbody);
            auto      data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msg_discoreq= reader.getRoot<riaps::discovery::DiscoRep>();


            // Get service
            if (msg_discoreq.isServiceLookup()) {
                auto msg_service_lookup = msg_discoreq.getServiceLookup();
                auto status = msg_service_lookup.getStatus();
                auto sockets = msg_service_lookup.getSockets();

                if (status == riaps::discovery::Status::OK) {
                    auto sockets = msg_service_lookup.getSockets();
                    for (auto it = sockets.begin(); it!=sockets.end();it++){

                        ServiceLookupResult result_item;

                        result_item.host_name = it->getHost();
                        result_item.port      = it->getPort();
                        result_item.part_name = part_name;
                        result_item.port_name = port_name;

                        result.push_back(result_item);
                    }
                }
            }

            /////
            /// Clean up
            /////

            zframe_destroy(&capnp_msgbody);
            zmsg_destroy(&msg_response);
            zsock_destroy(&client);

            return result;

        }

        bool Disco::JoinGroup(const std::string &app_name,
                              const std::string &component_id,
                              const riaps::groups::GroupId &group_id,
                              const std::vector<riaps::groups::GroupService> &group_services) {

            capnp::MallocMessageBuilder message;

            auto msgDiscoReq      = message.initRoot<riaps::discovery::DiscoReq>();
            auto msgGroupJoin     = msgDiscoReq.initGroupJoin();
            auto msgGroupId       = msgGroupJoin.initGroupId();
            auto msgGroupServices = msgGroupJoin.initServices(group_services.size());

            msgGroupJoin.setComponentId(component_id);
            msgGroupJoin.setAppName(app_name);
            msgGroupJoin.setPid(getpid());
            msgGroupId.setGroupType(group_id.group_type_id);
            msgGroupId.setGroupName(group_id.group_name);

            for (int i = 0; i< group_services.size(); i++){
                auto& gs = group_services[i];
                auto address = fmt::format("{}:{}", gs.host, gs.port);
                msgGroupServices[i].setAddress(address);
                msgGroupServices[i].setMessageType(gs.message_type);
            }

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t* msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            std::string ipcAddress = riaps::framework::Configuration::GetDiscoveryEndpoint();
            zsock_t * client = zsock_new_req (ipcAddress.c_str());
            assert(client);

            zmsg_send(&msg, client);

            /////
            /// Response
            /////
            zmsg_t* msgResponse = zmsg_recv(client);

            zframe_t* capnpBody = zmsg_pop(msgResponse);
            size_t    size = zframe_size(capnpBody);
            auto      data = zframe_data(capnpBody);

            auto capnpData = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnpData);
            auto msgRep= reader.getRoot<riaps::discovery::DiscoRep>();

            zsock_destroy(&client);
            zmsg_destroy(&msgResponse);
            zframe_destroy(&capnpBody);

            // If the response OK, return true
            return msgRep.isGroupJoin() && msgRep.getGroupJoin().getStatus() == riaps::discovery::Status::OK;
        }
    }
}







