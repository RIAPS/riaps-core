//
// Created by istvan on 3/15/17.
//

#ifndef RIAPS_CORE_SENSORQUERY_H
#define RIAPS_CORE_SENSORQUERY_H

#include "capnp/distributedestimator.capnp.h"
#include <componentmodel/r_messagebase.h>
#include <string>

namespace distributedestimator{
    namespace messages{
        class SensorQuery : public riaps::MessageBase{
        public:
            SensorQuery();
            //virtual void InitFields();

            void SetMsg(const std::string& msg);
            const std::string GetMsg();

            //virtual kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> GetBytes();

            ~SensorQuery();
        private:
            ::SensorQuery::Builder _builder;
            ::SensorQuery::Reader  _reader;

        };
    }
}



#endif //RIAPS_CORE_SENSORQUERY_H
