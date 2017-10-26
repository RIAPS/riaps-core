//
// Created by istvan on 10/26/17.
//

#ifndef RIAPS_CORE_R_REGISTRATION_H
#define RIAPS_CORE_R_REGISTRATION_H

#include <utils/r_expirablelist.h>

#include <opendht.h>

template <typename T>
struct ServiceInfo {
    std::string _key;
    T           _value;
    int         _pid;
};

template <typename T>
class ExpirableRegister{

public:
    ExpirableRegister(dht::DhtRunner& dhtNode) : _dhtNode(dhtNode){

    };

    void AddValue(const std::string& key, T& value, int pid) {
        ServiceInfo<T> si;
        si._key   = key;
        si._value = value;
        si._pid   = pid;

        _list.AddValue(si);
    }

    ~ExpirableRegister(){

    };

private:
    dht::DhtRunner& _dhtNode;
    ExpirableList<ServiceInfo<T>> _list;
};

#endif //RIAPS_CORE_R_REGISTRATION_H
