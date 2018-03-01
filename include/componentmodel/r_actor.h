//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_ACTOR_H
#define RIAPS_R_ACTOR_H

#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_peripheral.h>
#include <componentmodel/r_devmapi.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_deploapi.h>
#include <messaging/disco.capnp.h>
#include <const/r_const.h>
#include <utils/r_utils.h>
#include <groups/r_group.h>

#include <spdlog/spdlog.h>
#include <json.h>

#include <czmq.h>
#include <dlfcn.h>

#include <iostream>
#include <string>
#include <fstream>
#include <set>

namespace spd = spdlog;

namespace riaps {

    class ComponentBase;
    class Peripheral;

    namespace deplo {
        class DeploApi;
    }

    class Actor {
    public:


        static Actor* CreateActor(nlohmann::json& configJson   ,
                                  const std::string& actorName ,
                                  const std::string& jsonFile  ,
                                  std::map<std::string, std::string>& actualParams);

        static const Actor* GetRunningActor();

        bool init();
        virtual void start();
        std::string  getActorId();

        // Todo: Can they be static?
        // Todo: Actually a better question: can the whole actor be static?
        const std::string& getActorName() const;
        const std::string& getApplicationName() const;
        riaps::devm::DevmApi* getDeviceManager() const;
        riaps::deplo::DeploApi* getDeploManager() const;

        // Note: Group configs can be static
        // Todo: Thinking on to make them static... I'm not sure it is good.
        const std::vector<groupt_conf>& getGroupTypes() const;
        const groupt_conf* getGroupType(const std::string &groupTypeId) const;
        ComponentBase* getComponentByName(const std::string &componentName) const;

        virtual ~Actor();
        void updatePort(std::string &instancename, std::string &portname, std::string &host, int port);
        void updateGroup(zframe_t *capnpMessageBody, const std::string &sourceComponentId);

    protected:

        struct ActorProperties {
            std::string    actorName;
            std::string    jsonFile;
            nlohmann::json jsonActorConfig;
            nlohmann::json jsonInstances;
            nlohmann::json jsonDevicesconfig;
            nlohmann::json jsonComponentsconfig;
            nlohmann::json jsonGroups;
        };

        struct DeviceProperties {
            std::string    actorName;
            std::string    deviceName;
            nlohmann::json configJson;
            nlohmann::json jsonDevicesconfig;
        };

        std::unique_ptr<ActorProperties>  m_actorProperties;
        std::unique_ptr<DeviceProperties> m_deviceProperties;

        /**
         * Starting a regular actor, NOT a device component
         * @param applicationname RIAPS application name
         * @param actorname The actor to be started
         * @param jsonFile  JSON file name
         * @param jsonActorconfig
         * @param configJson
         * @param commandLineParams Actor arguments
         */
        Actor(const std::string&     applicationname       ,
              const std::string&     actorname             ,
              const std::string&     jsonFile              ,
              nlohmann::json&        jsonActorconfig       ,
              nlohmann::json&        configJson            ,
              std::map<std::string, std::string>& commandLineParams
        );

        /**
         * Starting device component
         * @param applicationname RIAPS application name
         * @param deviceName The device to be started (componentType)
         * @param configJson JSON representation of the config file
         * @param commandLineParams Actor arguments
         */
        Actor(const std::string&     applicationname       ,
              const std::string&     deviceName            ,
              nlohmann::json&        configJson            ,
              std::map<std::string, std::string>& commandLineParams
        );

        template<class T>
        bool parseConfig(){

        }

        std::set<std::string> getLocalMessageTypes(nlohmann::json &jsonLocals);



        // Channel for incomming controll messages (e.g.: restart component)
        zsock_t*                    m_actor_zsock;
        zsock_t*                    m_discovery_socket;


        int                         _actor_port;
        //std::string                 jsonFile;
        std::string                 _actor_endpoint;
        std::shared_ptr<zuuid_t> m_actor_id;
        //std::string                 actorName;
        std::string                 m_applicationName;
        std::vector<ComponentBase*> m_components;
        std::vector<Peripheral*>    m_peripherals;
        std::vector<void*>          m_component_dll_handles;
        //bool                        _startDevice; // The actor doesn;t start the device. The DeviceActor starts it only
        //std::string                 deviceName;  // If the DeviceActor starts the device, the device name is set here
                                                  // and not passed in parameters. Only the actorname comes from parameters.


        std::map<std::string, std::string>&   m_commandLineParams;
        std::unique_ptr<riaps::devm::DevmApi> m_devm;
        std::unique_ptr<riaps::deplo::DeploApi> m_deplo;

        //nlohmann::json  jsonComponentsconfig;
        //nlohmann::json  jsonDevicesconfig;
        //nlohmann::json  _jsonActorconfig;
        nlohmann::json  _jsonInternals;
        nlohmann::json  _jsonLocals;
        nlohmann::json  _jsonFormals;

        // Configurations
        ////

        void getPortConfigs(nlohmann::json &jsonPortsConfig, _component_conf &results);

        void parseGroups();


        std::vector<component_conf> m_component_configurations;
        std::vector<groupt_conf>    m_grouptype_configurations;
        std::set<std::string>       m_localMessageTypes;

        static Actor* CurrentActor;
        std::shared_ptr<spd::logger> m_logger;
    private:
        zpoller_t*    m_poller;



        bool isDeviceActor() const;
        bool isComponentActor() const;
    };
}

#endif //RIAPS_R_ACTOR_H
