//
// Created by istvan on 5/11/17.
//

#include <componentmodel/r_peripheral.h>

namespace riaps{
    Peripheral::Peripheral(Actor* parentActor)
            : m_state(Peripheral::STARTING),
              m_parentActor(parentActor) {
        m_state = Peripheral::INITIAL;
    }

    void Peripheral::Setup(const std::string &appName,
                           const std::string &modelName,
                           const std::string &typeName,
                           const std::map<std::string, std::string> &args) {

        //m_parentActor->getDeviceManager()->RegisterDevice(app_name, modelName, typeName, args);
        m_parentActor->getDeploManager()->registerDevice(appName, modelName, typeName, args);
        m_state = Peripheral::READY;
    }

    Peripheral::~Peripheral() {

    }
}