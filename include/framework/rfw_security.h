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

            /**
             * @return True if the security is turned on in the riaps.conf
             */
            static const bool                               HasSecurity();

            /**
             * @return Private key.
             */
            static std::shared_ptr<dht::crypto::PrivateKey> private_key();

            /**
             * @return Private key path.
             */
            static const std::string                        key_path();

            /**
             * @param app_name Name of the application
             * @return The curve key path of the application.
             */
            static const std::string                        curve_key_path(const std::string& app_name);

            /**
             * @param app_name Name of the application
             * @return The curve key of the application.
             */
            static zcert_t*                                 curve_key(const std::string& app_name);
        };
    }
}

#endif //RIAPS_CORE_RFW_SECURITY_H
