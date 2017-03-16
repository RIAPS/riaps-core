//
// Created by istvan on 3/15/17.
//

#ifndef RIAPS_CORE_ESTIMATE_H
#define RIAPS_CORE_ESTIMATE_H

#include <msgpack.hpp>
#include <string>


namespace distributedestimator{
    namespace messages{
        class Estimate{
        public:
            Estimate();

            void SetMsg(const std::string& msg);
            const std::string& GetMsg();

            MSGPACK_DEFINE(_msg);

            ~Estimate();
        private:
            std::string _msg;

        };
    }
}

#endif //RIAPS_CORE_ESTIMATE_H
