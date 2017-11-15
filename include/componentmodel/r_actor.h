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
#include <messaging/disco.capnp.h>
#include <const/r_const.h>
#include <utils/r_utils.h>
#include <groups/r_group.h>

#include <json.h>

#include <czmq.h>
#include <dlfcn.h>

#include <iostream>
#include <string>
#include <fstream>
#include <set>


namespace riaps {

    class ComponentBase;
    class Peripheral;

    class Actor {
    public:


        static Actor* CreateActor(nlohmann::json& configJson   ,
                                  const std::string& actorName ,
                                  const std::string& jsonFile  ,
                                  std::map<std::string, std::string>& actualParams);

        static const Actor& GetRunningActor();

        void Init();
        virtual void start();
        std::string  GetActorId();

        // Todo: Can they be static?
        // Todo: Actually a better question: can the whole actor be static?
        const std::string& GetActorName() const;
        const std::string& GetApplicationName() const;
        riaps::devm::DevmApi* GetDeviceManager() const;

        // Note: Group configs can be static
        // Todo: Thinking on to make them static... I'm not sure it is good.
        const std::vector<groupt_conf>& GetGroupTypes() const;
        const groupt_conf* GetGroupType(const std::string& groupTypeId) const;

        virtual ~Actor();
        void UpdatePort(std::string& instancename, std::string& portname, std::string& host, int port);
        void UpdateGroup(zframe_t* capnpMessageBody, const std::string& sourceComponentId);

    protected:

        Actor(const std::string&     applicationname       ,
              const std::string&     actorname             ,
              const std::string&     jsonFile              ,
              nlohmann::json&        jsonActorconfig       ,
              nlohmann::json&        configJson            ,
              std::map<std::string, std::string>& commandLineParams
        );

        void ParseConfig();

        std::set<std::string> GetLocalMessageTypes(nlohmann::json& jsonLocals);



        // Channel for incomming controll messages (e.g.: restart component)
        zsock_t*                    _actor_zsock;

        zsock_t*                    _discovery_socket;


        int                         _actor_port;
        std::string                 _jsonFile;
        std::string                 _actor_endpoint;
        zuuid_t*                    _actor_id;
        std::string                 _actorName;
        std::string                 _applicationName;
        std::vector<ComponentBase*> _components;
        std::vector<Peripheral*>    _peripherals;
        std::vector<void*>          _component_dll_handles;
        bool                        _startDevice; // The actor doesn;t start the device. The DeviceActor starts it only
        std::string                 _deviceName;  // If the DeviceActor starts the device, the device name is set here
                                                  // and not passed in parameters. Only the actorname comes from parameters.


        std::map<std::string, std::string>&   _commandLineParams;
        std::unique_ptr<riaps::devm::DevmApi> _devm;

        nlohmann::json  _jsonComponentsconfig;
        nlohmann::json  _jsonDevicesconfig;
        nlohmann::json  _jsonActorconfig;
        nlohmann::json  _jsonInstances;
        nlohmann::json  _jsonInternals;
        nlohmann::json  _jsonLocals;
        nlohmann::json  _jsonFormals;

        // Configurations
        ////


        std::vector<component_conf> _component_configurations;
        std::vector<groupt_conf>    _grouptype_configurations;
    private:
        zpoller_t*    _poller;
        static Actor* _currentActor;
    };
}

#endif //RIAPS_R_ACTOR_H
