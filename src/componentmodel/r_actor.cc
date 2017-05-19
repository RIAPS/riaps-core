//
// Created by parallels on 9/6/16.
//


#include <componentmodel/r_actor.h>
#include <const/r_jsonmodel.h>

#include <set>
#include <componentmodel/r_argumentparser.h>


namespace riaps {



    Actor* riaps::Actor::CreateActor(nlohmann::json&    configJson,
                                     const std::string& actorName ,
                                     const std::string& jsonFile  ,
                                     std::map<std::string, std::string>& actualParams) {
        std::string applicationName    = configJson[J_NAME];
        nlohmann::json jsonActors      = configJson[J_ACTORS];


        // Find the actor
        if (jsonActors.find(actorName)==jsonActors.end()){
            std::cerr << "Didn't find actor in the model file: " << actorName << std::endl;
            return NULL;
        }

        auto jsonCurrentActor = jsonActors[actorName];

        return new riaps::Actor(
                            applicationName,
                            actorName,
                            jsonFile,
                            jsonCurrentActor,
                            configJson,
                            //jsonComponents,
                            //jsonDevices,
                            //jsonMessages,
                            actualParams
                    );
    }

    std::set<std::string> riaps::Actor::GetLocalMessageTypes(nlohmann::json &jsonLocals) {
        std::set<std::string> results;
        for (auto it_local  = jsonLocals.begin();
             it_local != jsonLocals.end();
             it_local++){
            std::string local_type = (it_local.value())[J_TYPE];
            results.insert(local_type);
        }
        return results;
    }

    riaps::Actor::Actor(const std::string&     applicationname,
                        const std::string&     actorname      ,
                        const std::string&     jsonFile       ,
                        nlohmann::json&        jsonActorconfig,
                        nlohmann::json&        configJson     ,
                        std::map<std::string, std::string>& commandLineParams)
        : //_jsonComponentsconfig(jsonComponentsconfig),
          //_jsonDevicesconfig(jsonDevicesconfig),
          //_jsonActorconfig(jsonActorconfig),
          _commandLineParams(commandLineParams),
          _jsonFile(jsonFile)
    {
        _jsonActorconfig       = jsonActorconfig;
        _jsonComponentsconfig  = configJson[J_COMPONENTS];
        _jsonDevicesconfig     = configJson[J_DEVICES];
        //nlohmann::json jsonMessages    = configJson[J_MESSAGES];


        _actorName       = actorname;
        _applicationName = applicationname;

        _jsonInstances = jsonActorconfig[J_INSTANCES];
        _jsonInternals = jsonActorconfig[J_INTERNALS];
        _jsonLocals    = jsonActorconfig[J_LOCALS];
        _jsonFormals   = jsonActorconfig[J_FORMALS];
        _startDevice   = false;
    }

    void riaps::Actor::ParseConfig() {
        // Get the actor's components, if there is no component => Stop, Error.
        if(_jsonInstances.size()==0) {
            throw std::invalid_argument("No component instances defined for the actor. Check the configuration file!");
        }

        auto localMessageTypes = GetLocalMessageTypes(_jsonLocals);

        // Get the components and devices
        for (auto it_currentconfig =  _jsonInstances.begin();
             it_currentconfig != _jsonInstances.end();
             it_currentconfig++) {

            auto componentName = it_currentconfig.key();
            std::string componentType = (it_currentconfig.value())[J_TYPE];
            bool isDevice = false;

            auto jsonComponentConfig = _jsonComponentsconfig[componentType];


            if (jsonComponentConfig == NULL){
                jsonComponentConfig = _jsonDevicesconfig[componentType];
                if (jsonComponentConfig == NULL){
                    throw std::invalid_argument("Device/component has no config section: " + componentName);
                }
                isDevice = true;
            }

            // If the component is not a device, but the actor just starting devices now (beacuse it
            // started from the DeviceActor class, then skip parsing. Do not deal with components.
            if (_startDevice && !isDevice) continue;

            // Check if the component in the map already (wrong configuration)
            for (auto component_config : _component_configurations) {

                if (componentName != component_config.component_name) continue;

                // The same key already in the list. Stop, Error.
                throw std::invalid_argument("Component name is not unique in the configuration file");
            }

            // Store the componentname - componenttype pair
            _component_conf_j new_component_config;

            new_component_config.component_name = componentName;
            new_component_config.component_type = componentType;
            new_component_config.isDevice       = isDevice;

            ArgumentParser parser(_commandLineParams, _jsonActorconfig, _jsonComponentsconfig, _actorName);
            new_component_config.component_parameters = parser.Parse(componentName);

            // Get the ports
            auto json_portsconfig = jsonComponentConfig[J_PORTS];

            // Get the publishers
            if (json_portsconfig.count(J_PORTS_PUBS)!=0){
                auto json_pubports = json_portsconfig[J_PORTS_PUBS];
                for (auto it_pubport=json_pubports.begin();
                     it_pubport!=json_pubports.end();
                     it_pubport++){

                    auto pubportname = it_pubport.key();
                    auto pubporttype = it_pubport.value()[J_TYPE];



                    _component_port_pub_j newpubconfig;
                    newpubconfig.portName = pubportname;
                    newpubconfig.messageType = pubporttype;

                    // If the porttype is defined in the Local list
                    if (localMessageTypes.find(pubporttype) != localMessageTypes.end()){
                        newpubconfig.isLocal = true;
                    } else {
                        newpubconfig.isLocal = false;
                    }

                    new_component_config.component_ports.pubs.push_back(newpubconfig);
                }
            }

            // Parse subscribers from the config
            if (json_portsconfig.count(J_PORTS_SUBS)!=0){
                auto json_subports = json_portsconfig[J_PORTS_SUBS];
                for (auto it_subport = json_subports.begin();
                     it_subport != json_subports.end() ;
                     it_subport++){

                    auto subportname = it_subport.key();
                    auto subporttype = it_subport.value()[J_TYPE];

                    _component_port_sub_j newsubconfig;
                    newsubconfig.portName = subportname;
                    newsubconfig.messageType = subporttype;

                    // If the porttype is defined in the Local list
                    if (localMessageTypes.find(subporttype) != localMessageTypes.end()){
                        newsubconfig.isLocal = true;
                    } else {
                        newsubconfig.isLocal = false;
                    }

                    new_component_config.component_ports.subs.push_back(newsubconfig);
                }
            }

            // Parse request ports
            if (json_portsconfig.count(J_PORTS_REQS)!=0){
                auto json_reqports = json_portsconfig[J_PORTS_REQS];
                for (auto it_reqport = json_reqports.begin();
                     it_reqport != json_reqports.end() ;
                     it_reqport++){

                    auto reqportname = it_reqport.key();
                    std::string reqtype = it_reqport.value()[J_PORT_REQTYPE];
                    std::string reptype = it_reqport.value()[J_PORT_REPTYPE];
                    std::string messagetype = reqtype + "#" + reptype;

                    _component_port_req_j newreqconfig;
                    newreqconfig.portName = reqportname;
                    //newreqconfig.messageType = subporttype;
                    newreqconfig.req_type = reqtype;
                    newreqconfig.rep_type = reptype;
                    newreqconfig.messageType = messagetype;

                    // If the porttype is defined in the Local list
                    if (localMessageTypes.find(reqtype) != localMessageTypes.end()){
                        newreqconfig.isLocal = true;
                    } else {
                        newreqconfig.isLocal = false;
                    }

                    new_component_config.component_ports.reqs.push_back(newreqconfig);
                }
            }

            // Parse response ports
            if (json_portsconfig.count(J_PORTS_REPS)!=0){
                auto json_repports = json_portsconfig[J_PORTS_REPS];
                for (auto it_repport = json_repports.begin();
                     it_repport != json_repports.end() ;
                     it_repport++){

                    auto repportname = it_repport.key();
                    std::string reqtype = it_repport.value()[J_PORT_REQTYPE];
                    std::string reptype = it_repport.value()[J_PORT_REPTYPE];
                    std::string messagetype = reqtype + "#" + reptype;

                    _component_port_rep_j newrepconfig;
                    newrepconfig.portName = repportname;
                    newrepconfig.req_type = reqtype;
                    newrepconfig.rep_type = reptype;
                    newrepconfig.messageType = messagetype;

                    // If the porttype is defined in the Local list
                    if (localMessageTypes.find(reqtype) != localMessageTypes.end()){
                        newrepconfig.isLocal = true;
                    } else {
                        newrepconfig.isLocal = false;
                    }

                    new_component_config.component_ports.reps.push_back(newrepconfig);
                }
            }

            // Get the timers
            if (json_portsconfig.count(J_PORTS_TIMS)!=0){
                auto json_tims = json_portsconfig[J_PORTS_TIMS];
                for (auto it_tim = json_tims.begin();
                     it_tim != json_tims.end() ;
                     it_tim++){

                    auto timname = it_tim.key();
                    auto timperiod = it_tim.value()["period"];

                    _component_port_tim_j newtimconfig;
                    newtimconfig.portName = timname;
                    newtimconfig.period   = timperiod;

                    new_component_config.component_ports.tims.push_back(newtimconfig);
                }
            }

            _component_configurations.push_back(new_component_config);
        }
    }

    void riaps::Actor::Init() {

        ParseConfig();

        // unique id / run
        _actor_id = zuuid_new();

        _devm = std::unique_ptr<riaps::devm::DevmApi>(new riaps::devm::DevmApi());


        // Open actor REP socket for further communications
        _actor_zsock = zsock_new_rep("tcp://*:!");
        assert(_actor_zsock);
        _poller = zpoller_new(_actor_zsock, NULL);
        assert(_poller);

        // Register the actor in the discovery service
        _discovery_socket = register_actor(_applicationName, _actorName);


        if (_discovery_socket == NULL) {
            throw std::runtime_error("Actor - Discovery socket cannot be NULL after register_actor");
        }

        zpoller_add(_poller, _discovery_socket);

        // If there is a device, start the device manager client
        for (auto& component_config : _component_configurations) {
            if (component_config.isDevice) {

                _devm->RegisterActor(_actorName, _applicationName, "0");
                zpoller_add(_poller, _devm->GetSocket());
                break;
            }
        }

        // If no component in the actor => Stop, Error
        if (_component_configurations.empty()){
            throw std::invalid_argument("No components are defined in the configuration file.");
        }



        for (auto& component_config : _component_configurations){
            // Load the component library

            std::locale loc;
            std::string lowercaselibname;

            for(auto ch : component_config.component_type)
                lowercaselibname+=std::tolower(ch,loc);


            std::string component_library_name = "lib" + lowercaselibname + ".so";

            void *handle = dlopen(component_library_name.c_str(), RTLD_NOW);
            if (handle == NULL) {

                // TODO: pass in parameter what to do
                throw std::runtime_error("Cannot open library: " + component_library_name + " (" + dlerror() + ")");

                //Load a default implementation, for testing
                //riaps::ComponentBase* debug_component = new DebugComponent(component_config, *this);
               // _components.push_back(debug_component);
            }
            else {

                // It is not a device, start the component
                if (!component_config.isDevice || (component_config.isDevice && _startDevice)) {
                    _component_dll_handles.push_back(handle);
                    riaps::ComponentBase *(*create)(component_conf_j &, Actor &);
                    create = (riaps::ComponentBase *(*)(component_conf_j &, Actor &)) dlsym(handle, "create_component");
                    riaps::ComponentBase *component_instance = (riaps::ComponentBase *) create(component_config, *this);
                    _components.push_back(component_instance);
                }

                // If it is a device, but start the pheripheral first
                else if (component_config.isDevice && !_startDevice){
                    auto peripheral = new Peripheral(this);

                    peripheral->Setup(_applicationName, _jsonFile, _actorName, _commandLineParams);
                    _peripherals.push_back(peripheral);
                }

                else {

                }
            }
        }
    }

    riaps::devm::DevmApi* riaps::Actor::GetDeviceManager() const {
        return _devm.get();
    }

    const std::string& riaps::Actor::GetApplicationName() const {
        return _applicationName;
    }

    std::string riaps::Actor::GetActorId() {
        return std::string(zuuid_str(_actor_id));

    }

    const std::string& riaps::Actor::GetActorName() const {
        return _actorName;
    }

    void riaps::Actor::UpdatePort(std::string &instancename, std::string &portname, std::string &host, int port) {
        // Get the component
        for (std::vector<ComponentBase *>::iterator it = _components.begin(); it != _components.end(); it++) {
            if ((*it)->GetConfig().component_name == instancename) {
                riaps::ComponentBase *component_instance = (*it);
                zmsg_t *msg_portupdate = zmsg_new();

                zmsg_addstr(msg_portupdate, CMD_UPDATE_PORT);
                zmsg_addstr(msg_portupdate, portname.c_str());
                zmsg_addstr(msg_portupdate, host.c_str());
                zmsg_addstr(msg_portupdate, std::to_string(port).c_str());

                zmsg_send(&msg_portupdate, (*it)->GetZmqPipe());

                return;
            }
        }


    }


    void riaps::Actor::start(){
        while (!zsys_interrupted) {
            void *which = zpoller_wait(_poller, 2000);

            //std::cout << "." << std::flush;

            if (which == _actor_zsock) {
                // Maybe later, control messages to the actor
            } else if (which == _discovery_socket){
                zmsg_t* msg = zmsg_recv(which);

                if (!msg){
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                zframe_t* capnp_msgbody = zmsg_pop(msg);
                size_t    size = zframe_size(capnp_msgbody);
                byte*     data = zframe_data(capnp_msgbody);

                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

                capnp::FlatArrayMessageReader reader(capnp_data);
                auto msg_discoupd = reader.getRoot<riaps::discovery::DiscoUpd>();
                auto msg_client   = msg_discoupd.getClient();
                auto msg_socket   = msg_discoupd.getSocket();
                auto msg_scope    = msg_discoupd.getScope();

                std::string instance_name = msg_client.getInstanceName();
                std::string port_name     = msg_client.getPortName();
                std::string host          = msg_socket.getHost();
                int         port          = msg_socket.getPort();

                UpdatePort(instance_name, port_name, host, port);

                zmsg_destroy(&msg);
            }
            else if (_devm->GetSocket()!=NULL && which == _devm->GetSocket()){
                // Devm messages
                zmsg_t* msg = zmsg_recv(which);

                if (!msg){
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                zframe_t* capnp_msgbody = zmsg_pop(msg);
                size_t    size = zframe_size(capnp_msgbody);
                byte*     data = zframe_data(capnp_msgbody);

                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

                capnp::FlatArrayMessageReader reader(capnp_data);
            }
            else {
            }
        }
    }

    riaps::Actor::~Actor() {
        //deregister_actor(GetActorId());

        for (riaps::ComponentBase* component : _components){
            component->StopComponent();
        }

        for (riaps::ComponentBase* component : _components){
            delete component;
        }

        zpoller_destroy(&_poller);
        zuuid_destroy(&_actor_id);
        zsock_destroy(&_discovery_socket);
        zsock_destroy(&_actor_zsock);

        for (void* handle : _component_dll_handles){
            dlclose(handle);
        }
    }
}


