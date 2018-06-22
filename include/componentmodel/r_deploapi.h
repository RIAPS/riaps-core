//
// Created by istvan on 2/22/18.
//

#ifndef RIAPS_CORE_R_DEPLOAPI_H
#define RIAPS_CORE_R_DEPLOAPI_H

#include <componentmodel/r_componentbase.h>
#include <framework/rfw_configuration.h>
#include <messaging/deplo.capnp.h>
#include <czmq.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>


namespace spd = spdlog;

namespace riaps {

    class ComponentBase;

    namespace deplo {
        class DeploApi {
        public:
            DeploApi(std::shared_ptr<spd::logger>& t_logger);

            void registerActor(const std::string& appName,
                               const std::string& version,
                               const std::string& actorName,
                               int32_t pid);

            void registerDevice(const std::string& appName,
                                const std::string& modelName,
                                const std::string& typeName,
                                const std::map<std::string, std::string>& deviceArgs);
//
//            void unregisterDevice(const std::string& app_name,
//                                  const std::string& modelName,
//                                  const std::string& typeName);

            zsock_t* getPairSocket();

            void handleRes(riaps::deplo::ResMsg::Reader& resReader,
                           std::vector<riaps::ComponentBase*>& components);

            bool isDeploRunning() const;

            ~DeploApi();

        private:
            std::shared_ptr<zsock_t> m_socket;
            std::shared_ptr<zsock_t> m_pairSocket;
            std::shared_ptr<spd::logger> m_logger;
            bool m_isDeploRunning;
            int32_t m_deploPort;
        };
    }
}

#endif //RIAPS_CORE_R_DEPLOAPI_H
