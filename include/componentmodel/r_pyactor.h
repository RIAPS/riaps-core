//
// Created by istvan on 6/22/18.
//

#ifndef RIAPS_CORE_R_PYACTOR_H
#define RIAPS_CORE_R_PYACTOR_H

#include <string>
#include "r_configuration.h"

class PyActor{
public:
    PyActor(const std::string& application_name, const std::string& actor_name);

    const std::string& application_name() const;
    const std::string& actor_name() const;

    const GroupTypeConf* GetGroupType(const std::string &groupTypeId) const;

    ~PyActor() = default;

private:
    std::string application_name_;
    std::string actor_name_;

    std::vector<GroupTypeConf> grouptype_configurations_;

};

#endif //RIAPS_CORE_R_PYACTOR_H
