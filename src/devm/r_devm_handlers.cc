//
// Created by istvan on 5/5/17.
//

#include <devm/r_devm_handlers.h>
#include <framework/rfw_network_interfaces.h>
#include <iostream>


namespace riaps{
    namespace devm{

        std::map<std::string, actor_details_t*>* DevmHandler::_clients;
        zsock_t* DevmHandler::_serverPort;

        void DevmHandler::HandleDevmReq(const DevmReq::Reader &devmRequest,
                                        std::map<std::string, actor_details_t *> *clients,
                                        zsock_t *serverPort)  {
            DevmHandler::_clients    = clients;
            DevmHandler::_serverPort = serverPort;

            if (devmRequest.isActorReg()){
                auto actorRegReq = devmRequest.getActorReg();
                DevmHandler::HandleActorReg(actorRegReq);
            }
            else if (devmRequest.isDeviceReg()){
                auto deviceRegReq = devmRequest.getDeviceReg();
                HandleDeviceReq(deviceRegReq);
            }
            else if (devmRequest.isDeviceUnreg()){
                auto deviceUnreg = devmRequest.getDeviceUnreg();
                HandleDeviceUnreq(deviceUnreg);
            }
        }

        void DevmHandler::HandleActorReg(const ActorRegReq::Reader &actorRegReq) {

            std::string appName = actorRegReq.getAppName();
            std::string appVersion = actorRegReq.getVersion();
            std::string appActorName = actorRegReq.getActorName();

            std::cout << "HandleActorReg: " << appName << " " << appActorName << std::endl;

            int clientPort = DevmHandler::SetupClient(appName, appVersion, appActorName);

            capnp::MallocMessageBuilder builder;
            auto repMessage = builder.initRoot<DevmRep>();
            auto actRegRep  = repMessage.initActorReg();
            actRegRep.setStatus(Status::OK);
            actRegRep.setPort(clientPort);

            auto serializedMessage = capnp::messageToFlatArray(builder);

            zmsg_t* zmsg = zmsg_new();
            zmsg_pushmem(zmsg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&zmsg, DevmHandler::_serverPort);
        }

        /*
        Set up a new client of the devm service. The client actors are to register with
        the service using the 'server' (REQ/REP) socket. The service will then create a dedicated
        (PAIR) socket for the client to connect to. This socket is used as a private communication
                channel between a specific client actor and the service.
        */
        int DevmHandler::SetupClient(const std::string &appName,
                                     const std::string &appVersion,
                                     const std::string &appActorName) {

            zsock_t* pairSocket = zsock_new(ZMQ_PAIR);
            auto host = riaps::framework::Network::GetIPAddress();

            if (host == "") {
                throw std::runtime_error("DevmHandler couldn't create PAIR socket. Cannot find  available network interface.");
            }

            std::string pub_endpoint = "tcp://" + host + ":!";
            auto port = zsock_bind(pairSocket, pub_endpoint.c_str());
            auto actorDetail = new actor_details_t;
            actorDetail->port = port;
            actorDetail->socket = pairSocket;

            // Note: Why do we need this whole global local here? Devm always create local components. Or not?
            std::string clientKeyBase = "/" + appName + '/' + appActorName + "/";
            (*DevmHandler::_clients)[clientKeyBase] = actorDetail;

            std::string clientKeyLocal = clientKeyBase + riaps::framework::Network::GetMacAddressStripped();
            (*DevmHandler::_clients)[clientKeyLocal] = actorDetail;

            std::string clientKeyGlobal = clientKeyBase + riaps::framework::Network::GetIPAddress();
            (*DevmHandler::_clients)[clientKeyGlobal] = actorDetail;

            return port;
        }

        /*
            Handle the req for a device
            */
        void DevmHandler::HandleDeviceReq(const DeviceRegReq::Reader &deviceRegReq) {

            std::string appName = deviceRegReq.getAppName();
            std::string modelName = deviceRegReq.getModelName();
            std::string typeName = deviceRegReq.getTypeName();

            ::capnp::List< ::DeviceArg>::Reader args = deviceRegReq.getDeviceArgs();

            std::cout << "handleDeviceReq: " <<  appName << " " << typeName << " ";

            std::string cmdArgs = "";
            for (int i = 0; i<args.size(); i++ ){
                auto arg = args[i];
                std::string name(arg.getName());
                std::string value(arg.getValue());

                std::cout << name << " ";
                std::cout << value << " ";

                cmdArgs += "--" + name + " " + value + " ";
            }

            std::cout << std::endl;


            DevmHandler::StartDevice(appName, modelName, typeName, cmdArgs);

            capnp::MallocMessageBuilder builder;
            auto repMessage = builder.initRoot<DevmRep>();
            auto devRegRep  = repMessage.initDeviceReg();
            devRegRep.setStatus(Status::OK);

            auto serializedMessage = capnp::messageToFlatArray(builder);

            zmsg_t* zmsg = zmsg_new();
            zmsg_pushmem(zmsg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&zmsg, DevmHandler::_serverPort);
        }

        // Todo: implement \o/
        void DevmHandler::StartDevice(const std::string &appName, const std::string &modelName,
                                      const std::string &typeName, const std::string &cmdArgs) {

        }


        // Todo: implement \o/
        void DevmHandler::StopDevice(const std::string &appName, const std::string &modelName,
                                     const std::string &typeName) {

        }

        void DevmHandler::HandleDeviceUnreq(const DeviceUnregReq::Reader &deviceUnregReq) {
            /*
            Handle the 'unreq' (release) for a device
            */
            std::string appName = deviceUnregReq.getAppName();
            std::string modelName = deviceUnregReq.getModelName();
            std::string typeName = deviceUnregReq.getTypeName();

            std::cout << "handleDeviceUnreq: " << appName << " " << typeName <<std::endl;

            DevmHandler::StopDevice(appName, modelName, typeName);

            capnp::MallocMessageBuilder builder;
            auto repMessage = builder.initRoot<DevmRep>();
            auto devUnregRep  = repMessage.initDeviceUnreg();
            devUnregRep.setStatus(Status::OK);

            auto serializedMessage = capnp::messageToFlatArray(builder);

            zmsg_t* zmsg = zmsg_new();
            zmsg_pushmem(zmsg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&zmsg, DevmHandler::_serverPort);
        }
    }
}