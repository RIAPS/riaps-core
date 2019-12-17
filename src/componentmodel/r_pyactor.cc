//
// Created by istvan on 6/22/18.
//

#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <componentmodel/r_pyactor.h>
#include <framework/rfw_network_interfaces.h>

using namespace std;

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
    auto ip = riaps::framework::Network::GetIPAddress();

    std::stringstream ss(ip);
    std::string item;
    while (std::getline(ss, item, '.')) {
        auto n = stoi(item);
        actor_id_.push_back(n);
    }

    uint32_t lpid = getpid();
    for (int i = 3; i>=0; i--) {
        auto rem = lpid%255;
        actor_id_.insert(actor_id_.begin()+4, rem);
        lpid = lpid/255;
    }
}

std::string PyActor::actor_id_str() const {
    string result;
    for (auto c : actor_id_) {
        result += to_string(c);
    }
    return result;
}

std::vector<uint8_t> PyActor::actor_id() const {
    return actor_id_;
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