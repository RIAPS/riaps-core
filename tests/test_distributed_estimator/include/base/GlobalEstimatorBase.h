//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_GLOBALESTIMATORBASE_H
#define RIAPS_CORE_GLOBALESTIMATORBASE_H

#include "componentmodel/r_componentbase.h"

#define PORT_SUB_ESTIMATE "estimate"
#define PORT_TIMER_WAKEUP "wakeup"

class GlobalEstimatorBase : public riaps::ComponentBase {

public:

    GlobalEstimatorBase(_component_conf_j& config, riaps::Actor& actor);

    virtual void RegisterHandlers();

    virtual void OnEstimate(const std::string& messagetype,
                         std::vector<std::string>& msgFields,
                         riaps::ports::PortBase* port)=0;

    virtual void OnWakeup(const std::string& messagetype,
                         std::vector<std::string>& msgFields,
                         riaps::ports::PortBase* port)=0;

    virtual ~GlobalEstimatorBase();

private:
    std::unique_ptr<std::uniform_real_distribution<double>> unif;
    std::default_random_engine                              re;
};

#endif //RIAPS_CORE_GLOBALESTIMATORBASE_H
