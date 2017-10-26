//
// Created by istvan on 10/25/17.
//

#ifndef RIAPS_CORE_R_EXPIRABLELIST_H
#define RIAPS_CORE_R_EXPIRABLELIST_H

#include <utils/r_expirable.h>
#include <vector>


template <typename T>
class ExpiredIterator {

public:
    ExpiredIterator(typename std::vector<Expirable<T>>::iterator it,
    typename std::vector<Expirable<T>>::iterator end) : _it(it), _end(end) {
        while (true) {
            if (_it->HasExpired()) {
                break;
            } else if (_it == _end) {
                break;
            }
            _it++;
        }
    };

    bool operator != (const ExpiredIterator<T>& expiredIt) {
        return expiredIt._it != this->_it;
    };

    Expirable<T>& operator * () {
        return *_it;
    };

    Expirable<T>* operator->(){
        return &(*_it);
    }

    ExpiredIterator operator ++ () {
        while (true) {
            _it++;
            if (_it->HasExpired()) {
                return ExpiredIterator(_it, _end);
            } else if (_it == _end) {
                return ExpiredIterator(_it, _end);
            }
        }
    };

private:
    typename std::vector<Expirable<T>>::iterator _it;
    typename std::vector<Expirable<T>>::iterator _end;

};


template <typename T>
class ExpirableList {
public:
    ExpirableList(){};

    void AddValue(const T& value, const int64_t duration = 10000){
        Expirable<T> e(value, duration);
        AddValue(e);
    };

    void AddValue(Expirable<T>& value){
        _values.push_back(value);
    };

    ExpiredIterator<T> begin() {return ExpiredIterator<T>(_values.begin(), _values.end());};
    ExpiredIterator<T> end()   {return ExpiredIterator<T>(_values.end(), _values.end());};

    ~ExpirableList(){};

private:
    std::vector<Expirable<T>> _values;

};







#endif //RIAPS_CORE_R_EXPIRABLELIST_H
