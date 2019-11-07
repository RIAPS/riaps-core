
#ifndef RIAPS_CORE_R_PARAMS_H
#define RIAPS_CORE_R_PARAMS_H

#include <czmq.h>
#include <memory>

namespace riaps{

    /**
     * Parameters for handling messages on the Answer and Query ports.
     */
    class MessageParams{
    public:
        MessageParams(char**     originId,
                char**     requestId,
                zframe_t** timestamp);

        MessageParams(const std::string&  originId,
                char**              requestId,
                zframe_t**          timestamp);

        MessageParams(char**              requestId,
                      zframe_t**          timestamp);

        const std::string& GetOriginId()  const;
        const std::string& GetRequestId() const;
        const int64_t      GetTimestamp() const;
        bool               HasTimestamp() const;


        virtual ~MessageParams();

    private:
        void Init(char**     requestId, zframe_t** timestamp);

        std::string _originId;
        std::string _requestId;
        zframe_t*   _timestampFrame;
        int64_t     _timestamp;
        bool        _hasTimestamp;
    };
};

#endif //RIAPS_CORE_R_PARAMS_H
