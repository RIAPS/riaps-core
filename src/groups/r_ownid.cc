//
// Created by istvan on 12/16/19.
//
#include <groups/r_ownid.h>

using namespace std;

namespace riaps {
    namespace groups {
        void OwnId::data(char* data, size_t size) {
            data_.reserve(size);
            for (int i=0; i<size; i++) {
                data_[i] = data[i];
                strdata_+=to_string(data_[i]);
            }
        }

        std::vector<char> & OwnId::data() {
            return data_;
        }

        std::string OwnId::strdata() const {
            return strdata_;
        }
    }
}