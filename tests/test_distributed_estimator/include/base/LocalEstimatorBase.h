//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_LOCALESTIMATORBASE_H
#define RIAPS_CORE_LOCALESTIMATORBASE_H

#include "componentmodel/r_componentbase.h"

#include <string>
#include <vector>

// Name of the ports from the model file
#define PORT_SUB_READY    "ready"
#define PORT_REQ_QUERY    "query"
#define PORT_PUB_ESTIMATE "estimate"

class LocalEstimatorBase : public riaps::ComponentBase {

public:

    LocalEstimatorBase(_component_conf_j& config, riaps::Actor& actor);

    virtual void RegisterHandlers();

    virtual void OnReady(const std::string& messagetype,
                         std::vector<std::string>& msgFields,
                         riaps::ports::PortBase* port)=0;

    // I think we don't need handler for the request port. Request-Response should be sync anyway.
    // The real async is router-dealer
    //
    //virtual void OnQuery(const std::string& messagetype,
    //                     std::vector<std::string>& msgFields,
    //                     riaps::ports::PortBase* port)=0;

    virtual bool SendQuery(const std::string& message);

    virtual bool SendEstimate(const std::string& message);

    virtual bool RecvQuery(std::string& messageType, std::vector<std::string>& messageFields);

    virtual ~LocalEstimatorBase();

};

#endif //RIAPS_CORE_LOCALESTIMATORBASE_H
