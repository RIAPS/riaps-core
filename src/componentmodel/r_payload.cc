#include <componentmodel/r_payload.h>
#include <czmq.h>
#include <zmsg.h>
#include <zmq.h>

riaps::MessageParams::MessageParams(char     **originId,
                        char     **requestId,
                        zframe_t **timestamp) {

    Init(requestId, timestamp);

    _originId.assign(*originId);
    zstr_free(originId);
}

riaps::MessageParams::MessageParams(char     **requestId,
                                    zframe_t **timestamp) {

    Init(requestId, timestamp);
    _originId="";
}

riaps::MessageParams::MessageParams(const std::string &originId, char **requestId, zframe_t **timestamp) {
    Init(requestId, timestamp);

    _originId = originId;
}

void riaps::MessageParams::Init(char **requestId, zframe_t **timestamp) {
    _timestampFrame = *timestamp;
    _requestId.assign(*requestId);
    zstr_free(requestId);

    _timestamp = -1;
    _hasTimestamp = false;

    byte* frameData = zframe_data(_timestampFrame);
    size_t size = zframe_size(_timestampFrame);
    if (size>0){
        int64_t* tsPtr = reinterpret_cast<int64_t*>(frameData);
        _timestamp = *tsPtr;
        _hasTimestamp = true;
    }
}

const std::string& riaps::MessageParams::GetRequestId() const {
    return _requestId;
}

const std::string& riaps::MessageParams::GetOriginId() const {
    return _originId;
}

const int64_t riaps::MessageParams::GetTimestamp() const {
    return _timestamp;
}

bool riaps::MessageParams::HasTimestamp() const {
    return _hasTimestamp;
}

riaps::MessageParams::~MessageParams() {
    zframe_destroy(&_timestampFrame);
}