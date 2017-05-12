//
// Created by istvan on 5/5/17.
//

#include <device/r_deviceactor.h>
#include <const/r_jsonmodel.h>
#include <componentmodel/r_argumentparser.h>

namespace riaps{


    DeviceActor::DeviceActor(const std::string&     applicationName,
                             const std::string&     deviceActorName,
                             nlohmann::json&        jsonActorConfig,
                             nlohmann::json&        jsonDevicesConfig,
                             std::map<std::string, std::string>& commandLineParams) {

        _deviceActorName       = deviceActorName;
        _applicationName       = applicationName;

        auto jsonInstances  = jsonActorConfig[J_INSTANCES];
        auto jsonInternals  = jsonActorConfig[J_INTERNALS];
        auto jsonLocals     = jsonActorConfig[J_LOCALS];
        auto jsonFormals    = jsonActorConfig[J_FORMALS];

        // Get the actor's components, if there is no component => Stop, Error.
        if(jsonInstances.size()==0) {
            throw std::invalid_argument("No component instances defined for the  device actor. Check the configuration file!");
        }

        std::set<std::string> localMessageTypes = GetLocalMessageTypes(jsonLocals);

        // Get the devices
        for (auto it_currentconfig =  jsonInstances.begin();
             it_currentconfig != jsonInstances.end();
             it_currentconfig++) {

            auto deviceName = it_currentconfig.key();
            std::string deviceType = (it_currentconfig.value())[J_TYPE];

            // Check if the device is in the configure section.
            auto jsonDeviceConfig = jsonDevicesConfig[deviceType];

            // Check if the component in the map already (wrong configuration)
            for (auto component_config : _component_configurations) {

                if (deviceName != component_config.component_name) continue;

                // The same key already in the list. Stop, Error.
                throw std::invalid_argument("Component name is not unique in the configuration file");
            }

            device_conf_j newDeviceConfig;

            newDeviceConfig.component_name = deviceName;
            newDeviceConfig.component_type = deviceType;

            ArgumentParser parser(commandLineParams, jsonActorConfig, jsonDevicesConfig, _deviceActorName);
            newDeviceConfig.component_parameters = parser.Parse(deviceName);

            // Get the ports
            auto json_portsconfig = jsonDeviceConfig[J_PORTS];

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

                    newDeviceConfig.component_ports.pubs.push_back(newpubconfig);
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

                    newDeviceConfig.component_ports.subs.push_back(newsubconfig);
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

                    newDeviceConfig.component_ports.reqs.push_back(newreqconfig);
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

                    newDeviceConfig.component_ports.reps.push_back(newrepconfig);
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

                    newDeviceConfig.component_ports.tims.push_back(newtimconfig);
                }
            }

            _component_configurations.push_back(newDeviceConfig);
        }
    }

    DeviceActor::~DeviceActor() {

    }

    DeviceActor* DeviceActor::CreateDeviceActor(nlohmann::json &configJson,
                                  const std::string &deviceName,
                                  std::map<std::string, std::string> &actualParams) {

        std::string applicationName    = configJson["name"];
        nlohmann::json jsonDevices     = configJson["devices"];

        auto json_messages   = configJson["messages"];

        // Find the actor to be started
        if (jsonDevices.find(deviceName)==jsonDevices.end()){
            std::cerr << "Didn't find actor in the model file: " << deviceName << std::endl;
            return NULL;
        }

        auto jsonCurrentDevice = jsonDevices[deviceName];



    }
}