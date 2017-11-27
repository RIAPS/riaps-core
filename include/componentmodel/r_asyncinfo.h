
#ifndef RIAPS_CORE_R_ASYNCINFO_H
#define RIAPS_CORE_R_ASYNCINFO_H

#include <czmq.h>
#include <memory>

namespace riaps{
    struct AsyncInfo{
        AsyncInfo(const std::string& originId,
                  const std::string& messageId,
                  zmsg_t** created,
                  zmsg_t **expiration);

        const std::string OriginId;
        const std::string MessageId;
        zmsg_t*     Created;
        zmsg_t*     Expiration;

        ~AsyncInfo();

    };
};

#endif //RIAPS_CORE_R_ASYNCINFO_H
