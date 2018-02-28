//
// Created by istvan on 2/22/18.
//

#include <componentmodel/r_deploapi.h>
#include <utils/r_utils.h>
#include <capnp/message.h>

namespace riaps{
    namespace deplo{

        DeploApi::DeploApi(std::shared_ptr<spd::logger>& t_logger)
                : m_isDeploRunning(false), m_logger(t_logger) {

                m_socket = std::shared_ptr<zsock_t>(zsock_new(ZMQ_REQ), [](zsock_t* z){
                zsock_destroy(&z);
            });

            // recv timeout (just because it is not nice to be stopped by the missing deplo process)
            //zsock_set_rcvtimeo(m_socket.get(), 1000);
            //zsock_set_linger(m_socket.get(), 0);
            zsock_connect(m_socket.get(), "%s", riaps::framework::Configuration::GetDeploEndpoint().c_str());
        }

        void DeploApi::registerActor(const std::string &appName, const std::string &version,
                                     const std::string &actorName, int32_t pid) {

            capnp::MallocMessageBuilder builder;
            auto msgDeploReq = builder.initRoot<riaps::deplo::DeplReq>();
            auto msgActorReg = msgDeploReq.initActorReg();
            msgActorReg.setAppName(appName);
            msgActorReg.setActorName(actorName);
            msgActorReg.setPid(pid);
            msgActorReg.setVersion(version);

            zmsg_t* zmsgReq = nullptr;
            zmsgReq << builder;

            zmsg_send(&zmsgReq, m_socket.get());



            // If the deplo is not running then set m_deploRuns
            zmsg_t* zmsgRep = zmsg_recv(m_socket.get());

            if (zmsgRep == nullptr){
                m_logger->error("No response from deplo, further communications with deplo is suspended");
                m_isDeploRunning = false;
                return;
            } else {
                capnp::FlatArrayMessageReader* reader;
                zframe_t* frame = zmsg_pop(zmsgRep);
                *frame >> reader;

                auto msgDeploRep = reader->getRoot<riaps::deplo::DeplRep>();
                auto msgActorRep = msgDeploRep.getActorReg();
                if (msgActorRep.getStatus() == riaps::deplo::Status::OK){
                    m_deploPort = msgActorRep.getPort();
                    std::string deploPairEndpoint = fmt::format("tcp://127.0.0.1:{}",m_deploPort);
                    m_pairSocket = std::shared_ptr<zsock_t>(zsock_new(ZMQ_PAIR), [](zsock_t* z){
                        zsock_destroy(&z);
                    });
                    zsock_connect(m_pairSocket.get(), "%s", deploPairEndpoint.c_str());
                } else {
                    m_logger->error("Couldn't register actor in deplo");
                }

                delete reader;
                zframe_destroy(&frame);
                zmsg_destroy(&zmsgRep);
            }

        }

//        void DeploApi::registerDevice(const std::string &appName,
//                                      const std::string &modelName,
//                                      const std::string &typeName,
//                                      const std::map<std::string, std::string> &deviceArgs) {
//
//            capnp::MallocMessageBuilder builder;
//            auto msgDeploReq = builder.initRoot<riaps::deplo::DeplReq>();
//            auto msgDeviceReg = msgDeploReq.initDeviceReg();
//            msgDeviceReg.setAppName(appName);
//            msgDeviceReg.setModelName(modelName);
//            msgDeviceReg.setTypeName(typeName);
//            auto msgDevArgs = msgDeviceReg.initDeviceArgs(deviceArgs.size());
//
//            int idx = 0;
//            for (auto& arg : deviceArgs) {
//                msgDevArgs[idx].setName(arg.first);
//                msgDevArgs[idx].setValue(arg.second);
//                idx++;
//            }
//
//            zmsg_t* zmsgReq = nullptr;
//            zmsgReq << builder;
//            zmsg_send(&zmsgReq, m_socket.get());
//
//            // If the deplo is not running then set m_deploRuns
//            zmsg_t* zmsgRep = zmsg_recv(m_socket.get());
//
//            if (zmsgRep == nullptr){
//                m_logger->error("No response from deplo, further communications with deplo was suspended");
//                m_isDeploRunning = false;
//                return;
//            } else {
//                capnp::FlatArrayMessageReader* reader;
//                zframe_t* frame = zmsg_pop(zmsgRep);
//                *frame >> reader;
//
//                auto msgDeploRep = reader->getRoot<riaps::deplo::DeplRep>();
//                auto msgDevRep = msgDeploRep.getDeviceReg();
//                if (msgDevRep.getStatus() == riaps::deplo::Status::OK){
//                   // Nothing to do, we are happy
//                } else {
//                    m_logger->error("Couldn't register actor in deplo");
//                }
//
//                delete reader;
//                zframe_destroy(&frame);
//                zmsg_destroy(&zmsgRep);
//            }
//
//        }
//
//        void DeploApi::unregisterDevice(const std::string &appName,
//                                      const std::string &modelName,
//                                      const std::string &typeName) {
//
//            capnp::MallocMessageBuilder builder;
//            auto msgDeploReq = builder.initRoot<riaps::deplo::DeplReq>();
//            auto msgDeviceUnreg = msgDeploReq.initDeviceUnreg();
//            msgDeviceUnreg.setAppName(appName);
//            msgDeviceUnreg.setModelName(modelName);
//            msgDeviceUnreg.setTypeName(typeName);
//
//            zmsg_t* zmsgReq = nullptr;
//            zmsgReq << builder;
//            zmsg_send(&zmsgReq, m_socket.get());
//
//            // If the deplo is not running then set m_deploRuns
//            zmsg_t* zmsgRep = zmsg_recv(m_socket.get());
//
//            if (zmsgRep == nullptr){
//                m_logger->error("No response from deplo, further communications with deplo was suspended");
//                m_isDeploRunning = false;
//                return;
//            } else {
//                capnp::FlatArrayMessageReader* reader;
//                zframe_t* frame = zmsg_pop(zmsgRep);
//                *frame >> reader;
//
//                auto msgDeploRep = reader->getRoot<riaps::deplo::DeplRep>();
//                auto msgDevRep = msgDeploRep.getDeviceUnreg();
//                if (msgDevRep.getStatus() == riaps::deplo::Status::OK){
//                    // Nothing to do, we are happy
//                } else {
//                    m_logger->error("Couldn't register actor in deplo");
//                }
//
//                delete reader;
//                zframe_destroy(&frame);
//                zmsg_destroy(&zmsgRep);
//            }
//
//        }

        void DeploApi::handleRes(riaps::deplo::ResMsg::Reader& resReader,
                                 std::vector<riaps::ComponentBase*>& components) {

            for (auto comp : components) {
                if (resReader.isResCPUX()) {
                    auto msgResCPU = resReader.getResCPUX();
                    comp->handleCPULimit();
                } else if (resReader.isResMemX()) {
                    auto msgResMemX = resReader.getResMemX();
                    comp->handleMemLimit();
                } else if (resReader.isResNetX()) {
                    auto msgResNetX = resReader.getResNetX();
                    comp->handleNetLimit();
                } else if (resReader.isResSpcX()) {
                    auto msgResSpcX = resReader.getResSpcX();
                    comp->handleSpcLimit();
                } else {
                    m_logger->critical("ResMsg arrived from deplo, but no handler is implemented in the actor");
                }
            }
        }

        bool DeploApi::isDeploRunning() const {
            return m_isDeploRunning;
        }

        zsock_t* DeploApi::getPairSocket() {
            if (!isDeploRunning())
                return nullptr;
            return m_pairSocket.get();
        }

        DeploApi::~DeploApi() = default;

    }
}