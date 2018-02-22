#include <componentmodel/r_argumentparser.h>
#include <componentmodel/r_actor.h>
#include <csignal>

#define NO_GROUP_TEST

namespace riaps {

    const Actor* riaps::Actor::GetRunningActor() {
        return CurrentActor;
    }

    Actor* riaps::Actor::CreateActor(nlohmann::json&    configJson,
                                     const std::string& actorName ,
                                     const std::string& jsonFile  ,
                                     std::map<std::string, std::string>& actualParams) {
        if (CurrentActor == nullptr){
            std::string applicationName    = configJson[J_NAME];
            nlohmann::json jsonActors      = configJson[J_ACTORS];


            // Find the actor
            if (jsonActors.find(actorName)==jsonActors.end()){
                std::cerr << "Didn't find actor in the model file: " << actorName << std::endl;
                return NULL;
            }

            auto jsonCurrentActor = jsonActors[actorName];

            CurrentActor = new riaps::Actor(
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

        return CurrentActor;

    }

    std::set<std::string> riaps::Actor::getLocalMessageTypes(nlohmann::json &jsonLocals) {
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
        : m_commandLineParams(commandLineParams),
          m_discovery_socket(nullptr),
          m_actor_zsock(nullptr){
        m_actorProperties = nullptr;
        m_deviceProperties = std::unique_ptr<DeviceProperties>(new DeviceProperties());

        m_deviceProperties->actorName         = deviceName;
        m_deviceProperties->deviceName        = deviceName;
        m_deviceProperties->configJson        = configJson;
        m_deviceProperties->jsonDevicesconfig = configJson[J_DEVICES];

        m_applicationName = applicationname;
    }

    riaps::Actor::Actor(const std::string&     applicationname,
                        const std::string&     actorname      ,
                        const std::string&     jsonFile       ,
                        nlohmann::json&        jsonActorconfig,
                        nlohmann::json&        configJson     ,
                        std::map<std::string, std::string>& commandLineParams)
        : m_commandLineParams(commandLineParams),
          m_discovery_socket(nullptr),
          m_actor_zsock(nullptr)
    {

        m_deviceProperties = nullptr;
        m_actorProperties = std::unique_ptr<ActorProperties>(new ActorProperties());




#ifndef NO_GROUP_TEST
        // TODO: Remove this
        // Note: Group testing
        //////////////////////////

        _group_port_pub p;
        _group_port_sub s;

//        m_grouptype_configurations.push_back(
//                groupt_conf{
//                        "TestGroupId", //GroupId
//                        {}
//                }
//        );
//
//        p.portName="TestPubPortName";
//        p.messageType="TestPortType";
//        m_grouptype_configurations.back().groupTypePorts.pubs.push_back(p);
//
//        s.portName = "TestSubPortName";
//        s.messageType=p.messageType;
//        m_grouptype_configurations.back().groupTypePorts.subs.push_back(s);

        m_grouptype_configurations.push_back(
                groupt_conf{
                        "BackupGroup", //GroupId
                        {},
                        true // leader election enabled/disabled
                }
        );

//        p.portName="QueryOut";
//        p.messageType="QueryRequest";
//        m_grouptype_configurations.back().groupTypePorts.pubs.push_back(p);
//
//        s.portName = "QueryIn";
//        s.messageType=p.messageType;
//        m_grouptype_configurations.back().groupTypePorts.subs.push_back(s);
//
//        p.portName="ResponseOut";
//        p.messageType="Estimate";
//        m_grouptype_configurations.back().groupTypePorts.pubs.push_back(p);
//
//        s.portName = "ResponseIn";
//        s.messageType=p.messageType;
//        m_grouptype_configurations.back().groupTypePorts.subs.push_back(s);


        /////////////////////////
        //TODO: REMOVE LINES ABOVE AFTER TESTING
#endif


        m_actorProperties->jsonActorConfig   = jsonActorconfig;
        m_actorProperties->jsonFile          = jsonFile;
        m_actorProperties->actorName         = actorname;
        m_actorProperties->jsonInstances     = jsonActorconfig[J_INSTANCES];
        m_actorProperties->jsonDevicesconfig = configJson[J_DEVICES];
        m_actorProperties->jsonGroups        = configJson[J_GROUPS];
        m_actorProperties->jsonComponentsconfig  = configJson[J_COMPONENTS];

        //nlohmann::json jsonMessages    = configJson[J_MESSAGES];


        //actorName       = actorname;
        m_applicationName = applicationname;

        //jsonInstances = jsonActorconfig[J_INSTANCES];
        _jsonInternals = jsonActorconfig[J_INTERNALS];
        _jsonLocals    = jsonActorconfig[J_LOCALS];
        _jsonFormals   = jsonActorconfig[J_FORMALS];
        //_startDevice   = false;


    }

    void riaps::Actor::getPortConfigs(nlohmann::json &jsonPortsConfig, _component_conf &results) {
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
                if (m_localMessageTypes.find(pubporttype) != m_localMessageTypes.end() || results.isDevice){
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
                if (m_localMessageTypes.find(subporttype) != m_localMessageTypes.end() || results.isDevice){
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
                if (m_localMessageTypes.find(reqtype) != m_localMessageTypes.end() || results.isDevice){
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
                if (m_localMessageTypes.find(reqtype) != m_localMessageTypes.end() || results.isDevice){
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
                if (m_localMessageTypes.find(qrytype) != m_localMessageTypes.end() || results.isDevice){
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
                if (m_localMessageTypes.find(qrytype) != m_localMessageTypes.end() || results.isDevice){
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

    bool riaps::Actor::isComponentActor() const {
        return m_actorProperties != nullptr;
    }

    bool riaps::Actor::isDeviceActor() const {
        return m_deviceProperties != nullptr;
    }

    template<>
    bool riaps::Actor::parseConfig<Actor::DeviceProperties>() {
        auto prop = m_deviceProperties.get();

        // Get the device, read its properties
        if(prop->jsonDevicesconfig.size()==0) {
            m_logger->critical("No device configuration found, but device actor started.");
            return false;
        }

        auto jsonDeviceConfig = prop->jsonDevicesconfig[prop->deviceName];

        _component_conf newDeviceConfig;

        newDeviceConfig.component_name = prop->deviceName;
        newDeviceConfig.component_type = prop->deviceName;
        newDeviceConfig.isDevice       = true;

        DeviceArgumentParser parser(m_commandLineParams, jsonDeviceConfig);
        newDeviceConfig.component_parameters = parser.Parse(prop->deviceName);

        // Get the ports
        auto jsonPortsConfig = jsonDeviceConfig[J_PORTS];

        getPortConfigs(jsonPortsConfig, newDeviceConfig);

        m_component_configurations.push_back(newDeviceConfig);
    }


    template<>
    bool riaps::Actor::parseConfig<Actor::ActorProperties>() {
        // Get the actor's components, if there is no component => Stop, Error.

        auto prop = m_actorProperties.get();

        if(prop->jsonInstances.size()==0) {
            m_logger->critical("No component instances defined for the actor. Check the configuration file!");
            return false;
            //throw std::invalid_argument("No component instances defined for the actor. Check the configuration file!");
        }

        // Get the components and devices
        for (auto it_currentconfig =  prop->jsonInstances.begin();
                  it_currentconfig != prop->jsonInstances.end();
                  it_currentconfig++) {

            auto componentName = it_currentconfig.key();
            std::string componentType = (it_currentconfig.value())[J_TYPE];

            /**
             * Is the corrent component a device or not.
             * If a device, just read the necesseary config and start the peripheral.
             */
            bool isDevice = false;
            auto jsonComponentConfig = prop->jsonComponentsconfig[componentType];

            if (jsonComponentConfig == NULL){
                jsonComponentConfig = prop->jsonDevicesconfig[componentType];
                if (jsonComponentConfig == NULL){
                    m_logger->critical("Device/component has no config section: {}", componentName);
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
            //if (_startDevice && deviceName != componentName) continue;

            // Check if the component in the map already (wrong configuration)
            for (auto component_config : m_component_configurations) {

                if (componentName != component_config.component_name) continue;

                // The same key already in the list. Stop, Error.
                throw std::invalid_argument("Component name is not unique in the configuration file");
            }

            // Store the componentname - componenttype pair
            _component_conf new_component_config;

            new_component_config.component_name = componentName;
            new_component_config.component_type = componentType;
            new_component_config.isDevice       = isDevice;

            ComponentArgumentParser parser(m_commandLineParams, prop->jsonActorConfig, prop->jsonComponentsconfig, prop->actorName);
            new_component_config.component_parameters = parser.Parse(componentName);

            // Get the ports
            auto json_portsconfig = jsonComponentConfig[J_PORTS];

            getPortConfigs(json_portsconfig, new_component_config);

            m_component_configurations.push_back(new_component_config);
        }
        return true;
    }

    bool riaps::Actor::init() {

        m_logger = spd::get(getActorName());
        m_logger->set_level(spd::level::debug);
        m_localMessageTypes = getLocalMessageTypes(_jsonLocals);

        if (isComponentActor()){
            parseGroups();
            parseConfig<ActorProperties>();
        }
        else if (isDeviceActor())
            parseConfig<DeviceProperties>();
        else {
            m_logger->critical("Unexpected state in actor init. No Actor or Device properties.");
            return false;
        }

        // unique id / run
        m_actor_id = std::shared_ptr<zuuid_t>(zuuid_new(), [](zuuid_t* id){
            zuuid_destroy(&id);
        });

        // Open actor REP socket for further communications
        m_actor_zsock = zsock_new_rep("tcp://*:!");
        assert(m_actor_zsock);
        m_poller = zpoller_new(m_actor_zsock, NULL);


        m_devm  = std::unique_ptr<riaps::devm::DevmApi>  (new riaps::devm:: DevmApi());
        m_deplo = std::unique_ptr<riaps::deplo::DeploApi>(new riaps::deplo::DeploApi(m_logger));


        if (isDeviceActor()){
            // Devices are registered in the deplo by the devm, not by the actor
        } else {
            m_deplo->registerActor(getApplicationName(),
                                   "0",
                                   m_actorProperties->actorName,
                                   getpid());
        }


        if (m_deplo->isDeploRunning()) {
            zpoller_add(m_poller, m_deplo->getPairSocket());
        }


        // New api is czmq, ignore_interrupts is obsolote
        zpoller_set_nonstop(m_poller, true);
        assert(m_poller);

        // Register the actor in the discovery service
        m_discovery_socket = isDeviceActor()                                                ?
                            registerActor(m_applicationName, m_deviceProperties->deviceName):
                            registerActor(m_applicationName, m_actorProperties ->actorName );


        if (m_discovery_socket == NULL) {
            m_logger->error("Actor - Discovery socket cannot be NULL after register_actor");
            return false;
            //throw std::runtime_error("Actor - Discovery socket cannot be NULL after register_actor");
        }

        zpoller_add(m_poller, m_discovery_socket);

        // If there is a device, start the device manager client
        for(auto itConf = m_component_configurations.begin(); itConf!=m_component_configurations.end(); itConf++) {
            // If current component is a device, and the actor started from the DeviceActor, then register
            if (itConf->isDevice && isDeviceActor()) {

                /**
                 * Register actor in the device manager.
                 * The device manager starts
                 */
                m_devm->RegisterActor(m_deviceProperties->deviceName, m_applicationName, "0");
                zpoller_add(m_poller, m_devm->GetSocket());
                break;
            }
        }

        // If no component in the actor => Stop, Error
        //if (m_component_configurations.empty()){
        //    throw std::invalid_argument("No components are defined in the configuration file.");
       // }



        // Load and start the component library
        for(auto itConf = m_component_configurations.begin(); itConf!=m_component_configurations.end(); itConf++) {
            std::locale loc;
            std::string lowercaselibname;

            for(auto ch : itConf->component_type)
                lowercaselibname+=std::tolower(ch,loc);


            const std::string componentLibraryName = "lib" + lowercaselibname + ".so";
            const std::string appPath = GetAppPath(getApplicationName());

            void* dlOpenHandle = nullptr;

            // No environment variable set, let dlopen() find the component library
            if (appPath == ""){
                dlOpenHandle = dlopen(componentLibraryName.c_str(), RTLD_NOW);

                if (dlOpenHandle == nullptr) {
                    m_logger->error("Cannot open library: {}", componentLibraryName);
                    m_logger->error("dlerror(): {}", dlerror());

                    return false;
                    //throw std::runtime_error("Cannot open library");
                }
            } else {
                const std::string fullPath = appPath + "/" + componentLibraryName;
                dlOpenHandle = dlopen(fullPath.c_str(), RTLD_NOW);
                if (dlOpenHandle == nullptr){
                    m_logger->error("Cannot open library: {}", fullPath);
                    m_logger->error("dlerror(): {}", dlerror());
                    return false;
                }
                    //throw std::runtime_error("Cannot open library: " + fullPath + " (" + dlerror() + ")");
            }

            if (dlOpenHandle != nullptr) {

                // It is not a device, start the component
                if (!itConf->isDevice || (itConf->isDevice && isDeviceActor())) {
                    m_component_dll_handles.push_back(dlOpenHandle);
                    riaps::ComponentBase *(*create)(component_conf &, Actor &);
                    create = (riaps::ComponentBase *(*)(component_conf &, Actor &)) dlsym(dlOpenHandle, "create_component");
                    riaps::ComponentBase *component_instance = (riaps::ComponentBase *) create(*itConf, *this);
                    m_components.push_back(component_instance);
                }

                // If it is a device, then start the pheripheral first
                else if (itConf->isDevice && isComponentActor()){
                    auto peripheral = new Peripheral(this);
                    peripheral->Setup(m_applicationName, m_actorProperties->jsonFile, itConf->component_type, m_commandLineParams);
                    m_peripherals.push_back(peripheral);
                }

                else {

                }
            }

            
        }
        
        return true;
    }

    ComponentBase* riaps::Actor::getComponentByName(const std::string &componentName) const {
        for (auto it = m_components.begin(); it != m_components.end(); it++) {
            if ((*it)->GetConfig().component_name == componentName)
                return *it;
        }
        return nullptr;
    }
    
    const std::vector<groupt_conf>& riaps::Actor::getGroupTypes() const {
        return m_grouptype_configurations;
    }

    const groupt_conf* riaps::Actor::getGroupType(const std::string &groupTypeId) const {
//        std::vector<std::string> g{groupTypeId};
//
//        auto result =
//        std::find_first_of(m_grouptype_configurations.begin(),
//                           m_grouptype_configurations.end(),
//                           g.begin(),
//                           g.end(),
//                           [](const groupt_conf& g, const std::string& id){
//                               if (g.groupTypeId == id) return true;
//                               return false;
//                           });
//
//        if (result == m_grouptype_configurations.end()) return nullptr;
//        return &(*result);

        for (auto it = m_grouptype_configurations.begin(); it!=m_grouptype_configurations.end(); it++){
            auto qrvaanyad = *it;

            if (it->groupTypeId == groupTypeId)
                return &(*it);
        }
        return nullptr;
    }



    riaps::devm::DevmApi* riaps::Actor::getDeviceManager() const {
        return m_devm.get();
    }

    const std::string& riaps::Actor::getApplicationName() const {
        return m_applicationName;
    }

    std::string riaps::Actor::getActorId() {
        return std::string(zuuid_str(m_actor_id.get()));

    }

    const std::string& riaps::Actor::getActorName() const {
        if (isComponentActor()){
            if (m_actorProperties != nullptr){
                return m_actorProperties->actorName;
            }
        } else if (isDeviceActor()){
            if (m_deviceProperties != nullptr){
                return m_deviceProperties->actorName;
            }
        }
        m_logger->critical("Actor name is not set.");
    }

    void riaps::Actor::updatePort(std::string &instancename, std::string &portname, std::string &host, int port) {
        // Get the component
        for (std::vector<ComponentBase *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
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
            void *which = zpoller_wait(m_poller, 2000);

            //std::cout << "." << std::flush;

            if (which == m_actor_zsock) {
                // Maybe later, control messages to the actor
            } else if (which == m_discovery_socket){
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

                    updatePort(instance_name, port_name, host, port);


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
                    updateGroup(capnp_msgbody, sourceComponentId);
                }
                zframe_destroy(&capnp_msgbody);
                zmsg_destroy(&msg);
            }
            else if (m_devm->GetSocket()!=NULL && which == m_devm->GetSocket()){
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
            else if (which = m_deplo->getPairSocket()){
                zmsg_t* msg = zmsg_recv(which);

                if (msg == nullptr) {

                } else {
                    zframe_t* frame = zmsg_pop(msg);

                    capnp::FlatArrayMessageReader* reader;
                    *frame >> reader;

                    auto msgRes = reader->getRoot<riaps::deplo::ResMsg>();
                    m_deplo->handleRes(msgRes, m_components);

                    delete reader;
                    zframe_destroy(&frame);
                    zmsg_destroy(&msg);
                }
            }
            else {
            }
        }
    }

    void riaps::Actor::updateGroup(zframe_t *capnpMessageBody, const std::string &sourceComponentId){
        for (ComponentBase* component : m_components) {
            std::string componentInstanceId = component->GetCompUuid();

            // Do not send update to the component, because the services originates from this component.
            if (componentInstanceId == sourceComponentId) continue;

            // Doesn't change the ownership, in other words: the pointers are not released
            zsock_send(component->GetZmqPipe(), "sf", CMD_UPDATE_GROUP, capnpMessageBody);


            //component->UpdateGroup(msgGroupUpdate);
        }
    }

    void riaps::Actor::parseGroups() {
        const nlohmann::json& jGroups = m_actorProperties->jsonGroups;
        if (jGroups.size()!=0) {
            for (auto it_grp = jGroups.begin();
                 it_grp != jGroups.end();
                 it_grp++) {

                std::string groupType = it_grp.key();
                bool hasL = it_grp.value()[J_GROUP_LEADER];
                bool hasC   = it_grp.value()[J_GROUP_CONSENSUS];

                groupt_conf gc = {
                    groupType,
                    {},
                    hasC,
                    hasL
                };
                m_grouptype_configurations.push_back(gc);
            }
        }
    }



    riaps::Actor::~Actor() {

        // Deregister only, if the registration was successful
        if (m_discovery_socket!= nullptr)
            deregisterActor(getActorName(), getApplicationName());

        for (riaps::ComponentBase* component : m_components){
            m_logger->info("Stop component: {}", component->GetConfig().component_name);
            component->StopComponent();
        }

        for (riaps::ComponentBase* component : m_components){
            delete component;
        }

        zpoller_destroy(&m_poller);
        //zuuid_destroy(&m_actor_id);
        if (m_discovery_socket != nullptr)
            zsock_destroy(&m_discovery_socket);
        if (m_actor_zsock != nullptr)
            zsock_destroy(&m_actor_zsock);

        for (int i =0; i<m_component_dll_handles.size(); i++){
            dlclose(m_component_dll_handles[i]);
        }
        zclock_sleep(800);
    }

    riaps::Actor* riaps::Actor::CurrentActor = nullptr;
}


