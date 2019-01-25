#ifndef RIAPS_CORE_RFW_SECURITY_H
#define RIAPS_CORE_RFW_SECURITY_H


#include <const/r_const.h>
#include <INIReader.h>
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
        private:

        };
    }
}

#endif //RIAPS_CORE_RFW_SECURITY_H
