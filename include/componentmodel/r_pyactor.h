//
// Created by istvan on 6/22/18.
//

#ifndef RIAPS_CORE_R_PYACTOR_H
#define RIAPS_CORE_R_PYACTOR_H

#include <string>
#include <optional>
#include <unordered_map>
#include "r_configuration.h"

class PyActor{
public:
    PyActor(const std::string& application_name,
            const std::string& actor_name);

    const std::string& application_name() const;
    const std::string& actor_name() const;

    const GroupConf* GetGroupType(std::string &groupTypeId) const;
    void AddGroupTypes(const std::vector<GroupConf>& group_conf);

    ~PyActor() = default;

private:
    std::string application_name_;
    std::string actor_name_;

    std::unordered_map<std::string, GroupConf> group_configurations_;

};

#endif //RIAPS_CORE_R_PYACTOR_H
