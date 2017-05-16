//
// Created by istvan on 5/11/17.
//

#ifndef RIAPS_CORE_R_DEVMAPI_H
#define RIAPS_CORE_R_DEVMAPI_H

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <messaging/devm.capnp.h>
#include <czmq.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

namespace riaps {
    namespace devm{
        class DevmApi{
        public:
            DevmApi();
            ~DevmApi();

            bool RegisterActor(const std::string& actorName,
                               const std::string& appName,
                               const std::string& version);

            bool RegisterDevice(const std::string& appName,
                                const std::string& modelName,
                                const std::string& typeName,
                                const std::map<std::string, std::string>& args);

            bool UnregisterDevice(const std::string &appName,
                                  const std::string &modelName,
                                  const std::string &typeName);

            zsock_t* GetSocket();

        private:
            zsock_t* _socket;
        };
    }
}

zsock_t*
RegisterDevice(const std::string& appName,
               const std::string& modelName,
               const std::string& typeName,
               const std::map<std::string, std::string>& devArgs);



#endif //RIAPS_CORE_R_DEVMAPI_H
