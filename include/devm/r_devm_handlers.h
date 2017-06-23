//
// Created by istvan on 5/5/17.
//

#ifndef RIAPS_CORE_R_DEVM_HANDLERS_H
#define RIAPS_CORE_R_DEVM_HANDLERS_H

#include "../messaging/devm.capnp.h"
#include <discoveryd/r_discovery_types.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>
#include <map>
#include <string>
#include <functional>
#include <memory>

namespace riaps{
    namespace devm{
        class DevmHandler{
        public:

            static void HandleDevmReq(const DevmReq::Reader &devmRequest,
                                      std::map<std::string, actor_details_t *> *clients,
                                      zsock_t *serverPort);

            static void StopAllDevices();

        private:
            static int SetupClient(const std::string &appName,
                                   const std::string &appVersion,
                                   const std::string &appActorName);

            static void StartDevice(const std::string &appName,
                                    const std::string &modelName,
                                    const std::string &deviceName,
                                    const std::string &cmdArgs);

            static void StopDevice(const std::string& appName,
                                   const std::string& modelName,
                                   const std::string& typeName);



            static void HandleActorReg(const ActorRegReq::Reader &actorRegReq);
            static void HandleDeviceReq(const DeviceRegReq::Reader &deviceRegReq);
            static void HandleDeviceUnreq(const DeviceUnregReq::Reader &deviceUnregReq);

            static int StartExecutable(const std::string& command);

            //static void CleanClients

            static std::map<std::string, actor_details_t*>* _clients;
            static std::map<std::string, int>               _childThreads;
            static zsock_t* _serverPort;
        };
    }
}




#endif //RIAPS_CORE_R_DEVM_HANDLERS_H
