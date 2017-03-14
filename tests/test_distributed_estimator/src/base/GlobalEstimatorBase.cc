//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>

GlobalEstimatorBase::GlobalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config, actor) {

}

void GlobalEstimatorBase::RegisterHandlers() {
    RegisterHandler(PORT_SUB_ESTIMATE, reinterpret_cast<riaps::riaps_handler>(&GlobalEstimatorBase::OnEstimate));
    RegisterHandler(PORT_TIMER_WAKEUP, reinterpret_cast<riaps::riaps_handler>(&GlobalEstimatorBase::OnWakeup));
}

GlobalEstimatorBase::~GlobalEstimatorBase() {

}