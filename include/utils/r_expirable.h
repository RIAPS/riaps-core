//
// Created by istvan on 10/25/17.
//

#ifndef RIAPS_CORE_R_EXPIRABLE_H
#define RIAPS_CORE_R_EXPIRABLE_H

#include <memory>
#include <czmq.h>

template<typename T>
class Expirable{
public:
    Expirable(const T& value, const int64_t duration=1000) : _duration(duration) {
        _value = value;
        UpdateTimestamp();
    }

    Expirable(const Expirable<T>& other){
        _value         = other._value;
        _lastTimestamp = other._lastTimestamp;
        _duration      = other._duration;
    }

    void UpdateTimestamp(){
        _lastTimestamp = zclock_mono();
    };

    bool HasExpired() const {
        int64_t timestamp = zclock_mono();
        bool r = (timestamp - _lastTimestamp)>_duration;
        return r;
    };

    const T&   GetValue() const{
        return _value;
    };

    ~Expirable(){
    };

private:
    T                  _value;
    int64_t            _lastTimestamp;
    int64_t            _duration;
};

#endif //RIAPS_CORE_R_EXPIRABLE_H
