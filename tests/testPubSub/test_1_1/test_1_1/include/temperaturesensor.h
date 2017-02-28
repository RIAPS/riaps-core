//
// Created by istvan on 2/28/17.
//

#ifndef RIAPS_CORE_TEMPERATURESENSOR_H
#define RIAPS_CORE_TEMPERATURESENSOR_H

#define PORT_SUB_ESTIMATE "estimate"
#define PORT_TIMER_WAKEUP "wakeup"

using namespace riaps;

class TemperatureSensor : public riaps::ComponentBase {

public:

    TemperatureSensor(_component_conf_j& config, riaps::Actor& actor);

    virtual void OnMessageArrived(const std::string& messagetype,
                                  std::vector<std::string>& msgFields,
                                  ports::PortBase* port);

    virtual ~TemperatureSensor();

};

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);

#endif //RIAPS_CORE_TEMPERATURESENSOR_H
