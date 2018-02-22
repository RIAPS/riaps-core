//
// Created by istvan on 5/11/17.
//

#include <componentmodel/r_peripheral.h>

namespace riaps{
    Peripheral::Peripheral(Actor* parentActor)
            : _state(Peripheral::STARTING),
              _parentActor(parentActor) {
        _state = Peripheral::INITIAL;
    }

    void Peripheral::Setup(const std::string &appName,
                           const std::string &modelName,
                           const std::string &typeName,
                           const std::map<std::string, std::string> &args) {

        _parentActor->getDeviceManager()->RegisterDevice(appName, modelName, typeName, args);
        _state = Peripheral::READY;
    }

    Peripheral::~Peripheral() {

    }
}