#include <componentmodel/r_argumentparser.h>
#include <componentmodel/r_actor.h>

#define NO_GROUP_TEST

namespace riaps {

    const Actor& riaps::Actor::GetRunningActor() {
        return *_currentActor;
    }

    Actor* riaps::Actor::CreateActor(nlohmann::json&    configJson,
                                     const std::string& actorName ,
                                     const std::string& jsonFile  ,
                                     std::map<std::string, std::string>& actualParams) {
        if (_currentActor == nullptr){
            std::string applicationName    = configJson[J_NAME];
            nlohmann::json jsonActors      = configJson[J_ACTORS];


            // Find the actor
            if (jsonActors.find(actorName)==jsonActors.end()){
                std::cerr << "Didn't find actor in the model file: " << actorName << std::endl;
                return NULL;
            }

            auto jsonCurrentActor = jsonActors[actorName];

            _currentActor = new riaps::Actor(
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

        return _currentActor;

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

    riaps::Actor::Actor(const std::string &applicationname, const std::string &deviceName, nlohmann::json &configJson,
                        std::map<std::string, std::string> &commandLineParams)
        : _commandLineParams(commandLineParams),
          _discovery_socket(nullptr),
          _actor_zsock(nullptr){
        _actorProperties = nullptr;
        _deviceProperties = std::unique_ptr<DeviceProperties>(new DeviceProperties());

        _deviceProperties->_actorName         = deviceName;
        _deviceProperties->_deviceName        = deviceName;
        _deviceProperties->_configJson        = configJson;
        _deviceProperties->_jsonDevicesconfig = configJson[J_DEVICES];

        _applicationName = applicationname;
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
          //_jsonFile(jsonFile),
          _discovery_socket(nullptr),
          _actor_zsock(nullptr)
    {

        _deviceProperties = nullptr;
        _actorProperties = std::unique_ptr<ActorProperties>(new ActorProperties());




#ifndef NO_GROUP_TEST
        // TODO: Remove this
        // Note: Group testing
        //////////////////////////

        _group_port_pub p;
        _group_port_sub s;

//        _grouptype_configurations.push_back(
//                groupt_conf{
//                        "TestGroupId", //GroupId
//                        {}
//                }
//        );
//
//        p.portName="TestPubPortName";
//        p.messageType="TestPortType";
//        _grouptype_configurations.back().groupTypePorts.pubs.push_back(p);
//
//        s.portName = "TestSubPortName";
//        s.messageType=p.messageType;
//        _grouptype_configurations.back().groupTypePorts.subs.push_back(s);

        _grouptype_configurations.push_back(
                groupt_conf{
                        "BackupGroup", //GroupId
                        {},
                        false // leader election enabled/disabled
                }
        );

        p.portName="QueryOut";
        p.messageType="QueryRequest";
        _grouptype_configurations.back().groupTypePorts.pubs.push_back(p);

        s.portName = "QueryIn";
        s.messageType=p.messageType;
        _grouptype_configurations.back().groupTypePorts.subs.push_back(s);

        p.portName="ResponseOut";
        p.messageType="Estimate";
        _grouptype_configurations.back().groupTypePorts.pubs.push_back(p);

        s.portName = "ResponseIn";
        s.messageType=p.messageType;
        _grouptype_configurations.back().groupTypePorts.subs.push_back(s);


        /////////////////////////
        //TODO: REMOVE LINES ABOVE AFTER TESTING
#endif


        _actorProperties->_jsonActorConfig   = jsonActorconfig;
        _actorProperties->_jsonFile          = jsonFile;
        _actorProperties->_actorName         = actorname;
        _actorProperties->_jsonInstances     = jsonActorconfig[J_INSTANCES];
        _actorProperties->_jsonDevicesconfig = configJson[J_DEVICES];
        //_jsonActorconfig       = jsonActorconfig;
        _actorProperties->_jsonComponentsconfig  = configJson[J_COMPONENTS];

        //nlohmann::json jsonMessages    = configJson[J_MESSAGES];


        //_actorName       = actorname;
        _applicationName = applicationname;

        //_jsonInstances = jsonActorconfig[J_INSTANCES];
        _jsonInternals = jsonActorconfig[J_INTERNALS];
        _jsonLocals    = jsonActorconfig[J_LOCALS];
        _jsonFormals   = jsonActorconfig[J_FORMALS];
        //_startDevice   = false;


    }

    void riaps::Actor::GetPortConfigs(nlohmann::json& jsonPortsConfig, _component_conf& results) {
        if (jsonPortsConfig.count(J_PORTS_PUBS)!=0){
            auto json_pubports = jsonPortsConfig[J_PORTS_PUBS];
            for (auto it_pubport=json_pubports.begin();
                 it_pubport!=json_pubports.end();
                 it_pubport++){

                auto pubportname = it_pubport.key();
                auto pubporttype = it_pubport.value()[J_TYPE];



                _component_port_pub newpubconfig;
                newpubconfig.portName = pubportname;
                newpubconfig.messageType = pubporttype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(pubporttype) != _localMessageTypes.end() || results.isDevice){
                    newpubconfig.isLocal = true;
                } else {
                    newpubconfig.isLocal = false;
                }

                results.component_ports.pubs.push_back(newpubconfig);
            }
        }

        // Parse subscribers from the config
        if (jsonPortsConfig.count(J_PORTS_SUBS)!=0){
            auto json_subports = jsonPortsConfig[J_PORTS_SUBS];
            for (auto it_subport = json_subports.begin();
                 it_subport != json_subports.end() ;
                 it_subport++){

                auto subportname = it_subport.key();
                auto subporttype = it_subport.value()[J_TYPE];

                _component_port_sub newsubconfig;
                newsubconfig.portName = subportname;
                newsubconfig.messageType = subporttype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(subporttype) != _localMessageTypes.end() || results.isDevice){
                    newsubconfig.isLocal = true;
                } else {
                    newsubconfig.isLocal = false;
                }

                results.component_ports.subs.push_back(newsubconfig);
            }
        }

        // Parse request ports
        if (jsonPortsConfig.count(J_PORTS_REQS)!=0){
            auto json_reqports = jsonPortsConfig[J_PORTS_REQS];
            for (auto it_reqport = json_reqports.begin();
                 it_reqport != json_reqports.end() ;
                 it_reqport++){

                auto reqportname = it_reqport.key();
                std::string reqtype = it_reqport.value()[J_PORT_REQTYPE];
                std::string reptype = it_reqport.value()[J_PORT_REPTYPE];
                std::string messagetype = reqtype + "#" + reptype;

                _component_port_req newreqconfig;
                newreqconfig.portName = reqportname;
                //newreqconfig.messageType = subporttype;
                newreqconfig.req_type = reqtype;
                newreqconfig.rep_type = reptype;
                newreqconfig.messageType = messagetype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(reqtype) != _localMessageTypes.end() || results.isDevice){
                    newreqconfig.isLocal = true;
                } else {
                    newreqconfig.isLocal = false;
                }

                results.component_ports.reqs.push_back(newreqconfig);
            }
        }

        // Parse response ports
        if (jsonPortsConfig.count(J_PORTS_REPS)!=0){
            auto json_repports = jsonPortsConfig[J_PORTS_REPS];
            for (auto it_repport = json_repports.begin();
                 it_repport != json_repports.end() ;
                 it_repport++){

                auto repportname = it_repport.key();
                std::string reqtype = it_repport.value()[J_PORT_REQTYPE];
                std::string reptype = it_repport.value()[J_PORT_REPTYPE];
                std::string messagetype = reqtype + "#" + reptype;

                _component_port_rep newrepconfig;
                newrepconfig.portName = repportname;
                newrepconfig.req_type = reqtype;
                newrepconfig.rep_type = reptype;
                newrepconfig.messageType = messagetype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(reqtype) != _localMessageTypes.end() || results.isDevice){
                    newrepconfig.isLocal = true;
                } else {
                    newrepconfig.isLocal = false;
                }

                results.component_ports.reps.push_back(newrepconfig);
            }
        }

        // Parse query ports
        if (jsonPortsConfig.count(J_PORTS_QRYS)!=0){
            auto json_qryports = jsonPortsConfig[J_PORTS_QRYS];
            for (auto it_qryport = json_qryports.begin();
                 it_qryport != json_qryports.end() ;
                 it_qryport++){

                auto qryportname = it_qryport.key();
                std::string qrytype = it_qryport.value()[J_PORT_QRYTYPE];
                std::string anstype = it_qryport.value()[J_PORT_ANSTYPE];
                std::string messagetype = qrytype + "#" + anstype;

                _component_port_qry newqryconfig;
                newqryconfig.portName = qryportname;
                //newreqconfig.messageType = subporttype;
                newqryconfig.qry_type = qrytype;
                newqryconfig.ans_type = anstype;
                newqryconfig.messageType = messagetype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(qrytype) != _localMessageTypes.end() || results.isDevice){
                    newqryconfig.isLocal = true;
                } else {
                    newqryconfig.isLocal = false;
                }

                results.component_ports.qrys.push_back(newqryconfig);
            }
        }

        // Parse answer ports
        if (jsonPortsConfig.count(J_PORTS_ANSS)!=0){
            auto json_ansports = jsonPortsConfig[J_PORTS_ANSS];
            for (auto it_ansport = json_ansports.begin();
                 it_ansport != json_ansports.end() ;
                 it_ansport++){

                auto ansportname = it_ansport.key();
                std::string qrytype = it_ansport.value()[J_PORT_QRYTYPE];
                std::string anstype = it_ansport.value()[J_PORT_ANSTYPE];
                std::string messagetype = qrytype + "#" + anstype;

                _component_port_ans newansconfig;
                newansconfig.portName = ansportname;
                newansconfig.qry_type = qrytype;
                newansconfig.ans_type = anstype;
                newansconfig.messageType = messagetype;

                // If the porttype is defined in the Local list
                if (_localMessageTypes.find(qrytype) != _localMessageTypes.end() || results.isDevice){
                    newansconfig.isLocal = true;
                } else {
                    newansconfig.isLocal = false;
                }

                results.component_ports.anss.push_back(newansconfig);
            }
        }

        // Get the timers
        if (jsonPortsConfig.count(J_PORTS_TIMS)!=0){
            auto json_tims = jsonPortsConfig[J_PORTS_TIMS];
            for (auto it_tim = json_tims.begin();
                 it_tim != json_tims.end() ;
                 it_tim++){

                auto timname = it_tim.key();
                auto timperiod = it_tim.value()["period"];

                _component_port_tim newtimconfig;
                newtimconfig.portName = timname;
                newtimconfig.period   = timperiod;

                results.component_ports.tims.push_back(newtimconfig);
            }
        }

        // Get the inside ports
        if (jsonPortsConfig.count(J_PORTS_INSS)!=0){
            auto json_inss = jsonPortsConfig[J_PORTS_INSS];
            for (auto it_ins = json_inss.begin();
                 it_ins != json_inss.end() ;
                 it_ins++){

                auto insname = it_ins.key();
                //auto timperiod = it_tim.value()["period"];

                _component_port_ins newinsconfig;
                newinsconfig.portName = insname;

                results.component_ports.inss.push_back(newinsconfig);
            }
        }
    }

    bool riaps::Actor::IsComponentActor() const {
        return _actorProperties != nullptr;
    }

    bool riaps::Actor::IsDeviceActor() const {
        return _deviceProperties != nullptr;
    }

    template<>
    bool riaps::Actor::ParseConfig<Actor::DeviceProperties>() {
        auto prop = _deviceProperties.get();

        // Get the device, read its properties
        if(prop->_jsonDevicesconfig.size()==0) {
            _logger->critical("No device configuration found, but device actor started.");
            return false;
        }

        auto jsonDeviceConfig = prop->_jsonDevicesconfig[prop->_deviceName];

        _component_conf newDeviceConfig;

        newDeviceConfig.component_name = prop->_deviceName;
        newDeviceConfig.component_type = prop->_deviceName;
        newDeviceConfig.isDevice       = true;

        // TODO: param parsing for device
//        ArgumentParser parser(_commandLineParams, prop->_jsonActorConfig, prop->_jsonComponentsconfig, prop->_actorName);
//        new_component_config.component_parameters = parser.Parse(componentName);

        // Get the ports
        auto jsonPortsConfig = jsonDeviceConfig[J_PORTS];

        GetPortConfigs(jsonPortsConfig, newDeviceConfig);

        _component_configurations.push_back(newDeviceConfig);
    }


    template<>
    bool riaps::Actor::ParseConfig<Actor::ActorProperties>() {
        // Get the actor's components, if there is no component => Stop, Error.

        auto prop = _actorProperties.get();

        if(prop->_jsonInstances.size()==0) {
            _logger->critical("No component instances defined for the actor. Check the configuration file!");
            return false;
            //throw std::invalid_argument("No component instances defined for the actor. Check the configuration file!");
        }

        // Get the components and devices
        for (auto it_currentconfig =  prop->_jsonInstances.begin();
                  it_currentconfig != prop->_jsonInstances.end();
                  it_currentconfig++) {

            auto componentName = it_currentconfig.key();
            std::string componentType = (it_currentconfig.value())[J_TYPE];

            /**
             * Is the corrent component a device or not.
             * If a device, just read the necesseary config and start the peripheral.
             */
            bool isDevice = false;
            auto jsonComponentConfig = prop->_jsonComponentsconfig[componentType];

            if (jsonComponentConfig == NULL){
                jsonComponentConfig = prop->_jsonDevicesconfig[componentType];
                if (jsonComponentConfig == NULL){
                    _logger->critical("Device/component has no config section: {}", componentName);
                    return false;
                    //throw std::invalid_argument("Device/component has no config section: " + componentName);
                }
                isDevice = true;
            }

            // If the component is not a device, but the actor just starting devices now (beacuse it
            // started from the DeviceActor class, then skip parsing. Do not deal with regular components.
            //if (_startDevice && !isDevice) continue;

            // If the current component is not the device, then go to the next component.
            // This is because one actor instantiates one device
            //if (_startDevice && _deviceName != componentName) continue;

            // Check if the component in the map already (wrong configuration)
            for (auto component_config : _component_configurations) {

                if (componentName != component_config.component_name) continue;

                // The same key already in the list. Stop, Error.
                throw std::invalid_argument("Component name is not unique in the configuration file");
            }

            // Store the componentname - componenttype pair
            _component_conf new_component_config;

            new_component_config.component_name = componentName;
            new_component_config.component_type = componentType;
            new_component_config.isDevice       = isDevice;

            ArgumentParser parser(_commandLineParams, prop->_jsonActorConfig, prop->_jsonComponentsconfig, prop->_actorName);
            new_component_config.component_parameters = parser.Parse(componentName);

            // Get the ports
            auto json_portsconfig = jsonComponentConfig[J_PORTS];

            GetPortConfigs(json_portsconfig, new_component_config);

            _component_configurations.push_back(new_component_config);
        }
        return true;
    }

    bool riaps::Actor::Init() {

        _logger = spd::get(GetActorName());
        _logger->set_level(spd::level::debug);
        _localMessageTypes = GetLocalMessageTypes(_jsonLocals);

        if (IsComponentActor())
            ParseConfig<ActorProperties>();
        else if (IsDeviceActor())
            ParseConfig<DeviceProperties>();
        else {
            _logger->critical("Unexpected state in actor init. No Actor or Device properties.");
            return false;
        }

        // unique id / run
        _actor_id = zuuid_new();

        _devm = std::unique_ptr<riaps::devm::DevmApi>(new riaps::devm::DevmApi());

        // Open actor REP socket for further communications
        _actor_zsock = zsock_new_rep("tcp://*:!");
        assert(_actor_zsock);
        _poller = zpoller_new(_actor_zsock, NULL);

        // New api is czmq, ignore_interrupts is obsolote
        zpoller_set_nonstop(_poller, true);

        // Obsolote signal handling
        //zpoller_ignore_interrupts(_poller);
        assert(_poller);

        // Register the actor in the discovery service
        _discovery_socket = IsDeviceActor()                                                ?
                            registerActor(_applicationName, _deviceProperties->_deviceName):
                            registerActor(_applicationName, _actorProperties ->_actorName );


        if (_discovery_socket == NULL) {
            _logger->error("Actor - Discovery socket cannot be NULL after register_actor");
            return false;
            //throw std::runtime_error("Actor - Discovery socket cannot be NULL after register_actor");
        }

        zpoller_add(_poller, _discovery_socket);

        // If there is a device, start the device manager client
        //for (auto& component_config : _component_configurations) {

        for(auto itConf = _component_configurations.begin(); itConf!=_component_configurations.end(); itConf++) {
            // If current component is a device, and the actor started from the DeviceActor, then register
            if (itConf->isDevice && IsDeviceActor()) {

                /**
                 * Register actor in the device manager.
                 * The device manager starts
                 */
                _devm->RegisterActor(_deviceProperties->_deviceName, _applicationName, "0");
                zpoller_add(_poller, _devm->GetSocket());
                break;
            }
        }

        // If no component in the actor => Stop, Error
        //if (_component_configurations.empty()){
        //    throw std::invalid_argument("No components are defined in the configuration file.");
       // }



        //for (auto& component_config : _component_configurations){
            // Load the component library
        for(auto itConf = _component_configurations.begin(); itConf!=_component_configurations.end(); itConf++) {
            std::locale loc;
            std::string lowercaselibname;

            for(auto ch : itConf->component_type)
                lowercaselibname+=std::tolower(ch,loc);


            const std::string componentLibraryName = "lib" + lowercaselibname + ".so";
            const std::string appPath = GetAppPath(GetApplicationName());

            void* dlOpenHandle = nullptr;

            // No environment variable set, let dlopen() find the component library
            if (appPath == ""){
                dlOpenHandle = dlopen(componentLibraryName.c_str(), RTLD_NOW);

                if (dlOpenHandle == nullptr) {
                    _logger->error("Cannot open library: {}", componentLibraryName);
                    _logger->error("dlerror(): {}", dlerror());

                    return false;
                    //throw std::runtime_error("Cannot open library");
                }
            } else {
                const std::string fullPath = appPath + "/" + componentLibraryName;
                dlOpenHandle = dlopen(fullPath.c_str(), RTLD_NOW);
                if (dlOpenHandle == nullptr){
                    _logger->error("Cannot open library: {}", fullPath);
                    _logger->error("dlerror(): {}", dlerror());
                    return false;
                }
                    //throw std::runtime_error("Cannot open library: " + fullPath + " (" + dlerror() + ")");
            }

            if (dlOpenHandle != nullptr) {

                // It is not a device, start the component
                if (!itConf->isDevice || (itConf->isDevice && IsDeviceActor())) {
                    _component_dll_handles.push_back(dlOpenHandle);
                    riaps::ComponentBase *(*create)(component_conf &, Actor &);
                    create = (riaps::ComponentBase *(*)(component_conf &, Actor &)) dlsym(dlOpenHandle, "create_component");
                    riaps::ComponentBase *component_instance = (riaps::ComponentBase *) create(*itConf, *this);
                    _components.push_back(component_instance);
                }

                // If it is a device, then start the pheripheral first
                else if (itConf->isDevice && IsComponentActor()){
                    auto peripheral = new Peripheral(this);
                    peripheral->Setup(_applicationName, _actorProperties->_jsonFile, itConf->component_type, _commandLineParams);
                    _peripherals.push_back(peripheral);
                }

                else {

                }
            }

            
        }
        
        return true;
    }

    ComponentBase* riaps::Actor::GetComponentByName(const std::string &componentName) const {
        for (auto it = _components.begin(); it != _components.end(); it++) {
            if ((*it)->GetConfig().component_name == componentName)
                return *it;
        }
        return nullptr;
    }
    
    const std::vector<groupt_conf>& riaps::Actor::GetGroupTypes() const {
        return _grouptype_configurations;
    }

    const groupt_conf* riaps::Actor::GetGroupType(const std::string &groupTypeId) const {
        std::vector<std::string> g{groupTypeId};

        auto result =
        std::find_first_of(_grouptype_configurations.begin(),
                           _grouptype_configurations.end(),
                           g.begin(),
                           g.end(),
                           [](const groupt_conf& g, const std::string& id){
                               if (g.groupTypeId == id) return true;
                               return false;
                           });

        if (result == _grouptype_configurations.end()) return nullptr;
        return &(*result);
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
        if (IsComponentActor()){
            if (_actorProperties != nullptr){
                return _actorProperties->_actorName;
            }
        } else if (IsDeviceActor()){
            if (_deviceProperties != nullptr){
                return _deviceProperties->_actorName;
            }
        }
        _logger->critical("Actor name is not set.");
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
                auto msgDiscoUpd  = reader.getRoot<riaps::discovery::DiscoUpd>();

                if (msgDiscoUpd.isPortUpdate()) {
                    auto msgPortUpd = msgDiscoUpd.getPortUpdate();
                    auto msgClient = msgPortUpd.getClient();
                    auto msgSocket = msgPortUpd.getSocket();
                    auto msgScope = msgPortUpd.getScope();

                    std::string instance_name = msgClient.getInstanceName();
                    std::string port_name = msgClient.getPortName();
                    std::string host = msgSocket.getHost();
                    int port = msgSocket.getPort();

                    UpdatePort(instance_name, port_name, host, port);


                } else if (msgDiscoUpd.isGroupUpdate()){
                    auto msgGroupUpd = msgDiscoUpd.getGroupUpdate();

//                    std::cout << "Group update arrived in actor "
//                              << msgGroupUpd.getGroupId().getGroupType().cStr()
//                              << "::"
//                              << msgGroupUpd.getGroupId().getGroupName().cStr()
//                              <<std::endl;

//                    for (int i = 0; i<msgGroupUpd.getServices().size(); i++){
//                        auto v = msgGroupUpd.getServices()[i];
//                        std::cout << " -" << v.getAddress().cStr() << "#" << v.getMessageType().cStr() << std::endl;
//                    }

                    std::string sourceComponentId = msgGroupUpd.getComponentId().cStr();
                    UpdateGroup(capnp_msgbody, sourceComponentId);
                }
                zframe_destroy(&capnp_msgbody);
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

    void riaps::Actor::UpdateGroup(zframe_t* capnpMessageBody, const std::string& sourceComponentId){
        for (ComponentBase* component : _components) {
            std::string componentInstanceId = component->GetCompUuid();

            // Do not send update to the component, because the services originates from this component.
            if (componentInstanceId == sourceComponentId) continue;

            // Doesn't change the ownership, in other words: the pointers are not released
            zsock_send(component->GetZmqPipe(), "sf", CMD_UPDATE_GROUP, capnpMessageBody);


            //component->UpdateGroup(msgGroupUpdate);
        }
    }

    riaps::Actor::~Actor() {

        // Deregister only, if the registration was successful
        if (_discovery_socket!= nullptr)
            deregisterActor(GetActorName(), GetApplicationName());

        for (riaps::ComponentBase* component : _components){
            _logger->info("Stop component: {}", component->GetConfig().component_name);
            component->StopComponent();
        }

        for (riaps::ComponentBase* component : _components){
            delete component;
        }

        zpoller_destroy(&_poller);
        zuuid_destroy(&_actor_id);
        if (_discovery_socket != nullptr)
            zsock_destroy(&_discovery_socket);
        if (_actor_zsock != nullptr)
            zsock_destroy(&_actor_zsock);

        for (int i =0; i<_component_dll_handles.size(); i++){
            dlclose(_component_dll_handles[i]);
        }
        zclock_sleep(800);
    }

    riaps::Actor* riaps::Actor::_currentActor = nullptr;
}


