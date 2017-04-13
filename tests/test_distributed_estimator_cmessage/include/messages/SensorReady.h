//
// Created by istvan on 3/14/17.
//

#ifndef RIAPS_CORE_SENSORREADY_H
#define RIAPS_CORE_SENSORREADY_H

#include "capnp/distributedestimator.capnp.h"
#include <componentmodel/r_messagebase.h>

#include <vector>
#include <string>
#include <algorithm>
#include <memory>


namespace distributedestimator {
    namespace messages {

        class SensorReady : public riaps::MessageBase {
        public:

            /// Default constructor. Use for building messages (for serialization).
            SensorReady();

            virtual void InitReader(capnp::FlatArrayMessageReader* flatArrayMessageReader);

            // Use this constructor for deserializing messages. (capnp reader message)
            void SetMsg(const std::string &msg);
            const std::string GetMsg();

            ~SensorReady();

        private:
            ::SensorReady::Builder _builder;
            ::SensorReady::Reader  _reader;
        };
    }
}

#endif //RIAPS_CORE_SENSORREADY_H
