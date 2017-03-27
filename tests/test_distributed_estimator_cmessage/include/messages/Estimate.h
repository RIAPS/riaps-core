//
// Created by istvan on 3/15/17.
//

#ifndef RIAPS_CORE_ESTIMATE_H
#define RIAPS_CORE_ESTIMATE_H

#include "capnp/distributedestimator.capnp.h"
#include <componentmodel/r_messagebase.h>
#include <string>


namespace distributedestimator{
    namespace messages{

        class Estimate : public riaps::MessageBase {
        public:
            Estimate();
            //virtual void InitFields();

            void SetMsg(const std::string& msg);
            const std::string GetMsg();

            std::vector<double>& GetData();

            //virtual kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> GetBytes();


            ~Estimate();
        private:
            ::Estimate::Builder&& _builder;

        };
    }
}

#endif //RIAPS_CORE_ESTIMATE_H
