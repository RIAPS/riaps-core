#ifndef RIAPS_CORE_RFW_SECURITY_H
#define RIAPS_CORE_RFW_SECURITY_H


#include <const/r_const.h>
#include <INIReader.h>
#include <czmq.h>
#include <opendht.h>
#include <string>
#include <memory>

namespace riaps {
    namespace framework {
        class Security {
        public:
            static const bool                               HasSecurity();
            static std::shared_ptr<dht::crypto::PrivateKey> private_key();
            static const std::string                        key_path();

            static const std::string                        curve_key_path(const std::string& app_name);
            static zcert_t*                                 curve_key(const std::string& app_name);
        private:

        };
    }
}

#endif //RIAPS_CORE_RFW_SECURITY_H
