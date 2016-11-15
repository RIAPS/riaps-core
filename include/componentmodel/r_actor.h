//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_ACTOR_H
#define RIAPS_R_ACTOR_H

#include <czmq.h>
#include <iostream>
#include <string>
#include <dlfcn.h>


#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_componentbase.h>

#include <fstream>
#include <json.h>


namespace riaps {

    class ComponentBase;

    class Actor {
    public:
        Actor(const std::string& applicationname,
              const std::string& actorname,
              nlohmann::json& json_actorconfig,
              nlohmann::json& json_componentsconfig,
              nlohmann::json& json_messagesconfig)
        ;

        void Init();
        virtual void start();
        std::string GetActorId();
        const std::string& GetApplicationName() const;
        virtual ~Actor();

    protected:
        zpoller_t*                 _poller;

        // Channel for incomming controll messages (e.g.: restart component)
        zsock_t*                   _actor_zsock;

        zsock_t*                   _discovery_socket;

        int                         _actor_port;
        std::string                 _actor_endpoint;
        zuuid_t*                    _actor_id;
        std::string                 _actor_name;
        std::string                 _application_name;
        std::vector<ComponentBase*> _components;
        std::vector<void*>          _component_dll_handles;

        // Configurations
        ////


        std::vector<component_conf_j> _component_configurations;

        //Components, componentkey - componenttype
        //std::map<std::string, std::string> _componentname_type;
    };
}

#endif //RIAPS_R_ACTOR_H
