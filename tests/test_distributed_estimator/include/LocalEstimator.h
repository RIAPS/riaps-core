//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_LOCALESTIMATOR_H
#define RIAPS_FW_LOCALESTIMATOR_H


#include "base/LocalEstimatorBase.h"


class LocalEstimator : public LocalEstimatorBase {

public:

    LocalEstimator(_component_conf_j& config, riaps::Actor& actor);

    virtual void OnReady(const std::string& messagetype,
                         std::vector<std::string>& msgFields,
                         riaps::ports::PortBase* port);


    virtual ~LocalEstimator();

private:
    std::unique_ptr<std::uniform_real_distribution<double>> unif;
    std::default_random_engine                              re;
};

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_LOCALESTIMATOR_H
