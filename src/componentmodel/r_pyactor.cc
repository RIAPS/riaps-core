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

PyActor::PyActor(const std::string &application_name, const std::string &actor_name)
    : application_name_(application_name),
      actor_name_(actor_name) {

}

const groupt_conf* PyActor::GetGroupType(const std::string &groupTypeId) const {
    for (auto it = grouptype_configurations_.begin(); it != grouptype_configurations_.end(); it++) {
        if (it->groupTypeId == groupTypeId)
            return &(*it);
    }
    return nullptr;
}