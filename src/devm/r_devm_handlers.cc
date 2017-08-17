//
// Created by istvan on 5/5/17.
//

#include <devm/r_devm_handlers.h>
#include <framework/rfw_network_interfaces.h>
#include <iostream>
#include <cstdlib>


namespace riaps{
    namespace devm{

        std::map<std::string, actor_details_t*>* DevmHandler::_clients;
        zsock_t*                                 DevmHandler::_serverPort;
        std::map<std::string, int>               DevmHandler::_childThreads;

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

            //std::string clientKeyLocal = clientKeyBase + riaps::framework::Network::GetMacAddressStripped();
            //(*DevmHandler::_clients)[clientKeyLocal] = actorDetail;

            //std::string clientKeyGlobal = clientKeyBase + riaps::framework::Network::GetIPAddress();
            //(*DevmHandler::_clients)[clientKeyGlobal] = actorDetail;

            return port;
        }

        /*
            Handle the req for a device
            */
        void DevmHandler::HandleDeviceReq(const DeviceRegReq::Reader &deviceRegReq) {

            std::string appName = deviceRegReq.getAppName();
            std::string modelName = deviceRegReq.getModelName();
            std::string deviceName = deviceRegReq.getTypeName();

            ::capnp::List< riaps::devm::DeviceArg>::Reader args = deviceRegReq.getDeviceArgs();

            std::cout << "handleDeviceReq: " <<  appName << " " << deviceName << " ";

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


            DevmHandler::StartDevice(appName, modelName, deviceName, cmdArgs);

            capnp::MallocMessageBuilder builder;
            auto repMessage = builder.initRoot<DevmRep>();
            auto devRegRep  = repMessage.initDeviceReg();
            devRegRep.setStatus(Status::OK);

            auto serializedMessage = capnp::messageToFlatArray(builder);

            zmsg_t* zmsg = zmsg_new();
            zmsg_pushmem(zmsg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&zmsg, DevmHandler::_serverPort);
        }

        void DevmHandler::StartDevice(const std::string &appName,
                                      const std::string &modelName,
                                      const std::string &deviceName,
                                      const std::string &cmdArgs) {

            // TODO: do not hardcode the path, worst solution ever, but helps to test.
            std::string command = "/home/istvan/work/riaps-core/bin/start_device";

            // Add model - skip appName, we don't need for that
            command += " " + modelName; // model.json
            command += " " + deviceName;
            command += " " + cmdArgs;

            int childPid = StartExecutable(command);
            if (childPid == -1){
                std::cerr << "Failed to start: " << command <<std::endl;
            } else {
                std::string key = appName + "." + deviceName;
                _childThreads[key] = childPid;
            }
        }

        int DevmHandler::StartExecutable(const std::string &command) {
            auto pid = fork();
            if (pid < 0) return -1; //fork failed

            // Id the pid 0, we are in the child process, start the device
            if (pid == 0) {
                std::system(command.c_str());
                exit(1);
            }
            // Parent process returns the PID of the child
            else {
                return pid;
            }
        }


        void DevmHandler::StopDevice(const std::string &appName, const std::string &modelName,
                                     const std::string &typeName) {
            std::string key = appName + "." + typeName;
            if (_childThreads.find(key)!=_childThreads.end()){
                auto childPid = _childThreads[key];
                kill(childPid, SIGINT);
                _childThreads.erase(key);
            }
        }

        void DevmHandler::StopAllDevices() {
            for (auto it = _childThreads.begin(); it!=_childThreads.end(); it++){
                auto pid = it->second;
                std::cout << "Kill: " << pid << std::endl;
                kill(pid, SIGINT);
                zclock_sleep(500);
            }
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