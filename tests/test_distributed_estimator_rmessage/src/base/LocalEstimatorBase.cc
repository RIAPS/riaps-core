//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>

LocalEstimatorBase::LocalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config, actor){

}

void LocalEstimatorBase::RegisterHandlers() {
    RegisterHandler(PORT_SUB_READY, reinterpret_cast<riaps::riaps_handler>(&LocalEstimatorBase::OnReady));
}

LocalEstimatorBase::~LocalEstimatorBase() {

}