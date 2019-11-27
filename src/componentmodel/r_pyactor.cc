//
// Created by istvan on 6/22/18.
//

#include <componentmodel/r_pyactor.h>

const std::string& PyActor::application_name() const {
    return application_name_;
}

const std::string& PyActor::actor_name() const {
    return actor_name_;
}

PyActor::PyActor(const std::string &application_name,
                 const std::string &actor_name)
    : application_name_(application_name),
      actor_name_(actor_name) {

}

void PyActor::AddGroupTypes(const std::vector<GroupConf> &group_conf) {
    for (auto& gconf : group_conf) {
        group_configurations_[gconf.name()] = gconf;
    }
}

const GroupConf* PyActor::GetGroupType(std::string &name) const {
    if (group_configurations_.find(name) == group_configurations_.end())
        return nullptr;
    return &(group_configurations_.at(name));
}