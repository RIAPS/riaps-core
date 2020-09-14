//
// Created by istvan on 12/16/19.
//
#include <groups/r_ownid.h>



using namespace std;
namespace spd = spdlog;
namespace riaps {
    namespace groups {
//        OwnId::OwnId(const riaps::groups::OwnId &rhs) {
//            this->data_ = rhs.data_;
//            this->strdata_ = rhs.strdata_;
//        }
//
//        OwnId OwnId::operator=(const OwnId &rhs) {
//            OwnId tmp;
//            tmp.data_ = rhs.data_;
//            tmp.strdata_ = rhs.strdata_;
//            return tmp;
//        }

        void OwnId::data(uint8_t* data, size_t size) {
            //data_.reserve(size);
            data_ = vector<uint8_t>(size, 0);
            for (int i=0; i<size; i++) {
                data_[i] = data[i];
                strdata_+=to_string(data_[i]);
            }
        }

        void OwnId::data(const vector<uint8_t> & actorid, uint64_t group_address) {
            uint8_t ownid[16];
            for (int i = 0; i<8; i++) {
                ownid[i] = actorid[i];
            }

            for (int i = 8; i<16; i++) {
                ownid[i] = group_address%256;
                group_address = group_address/256;
            }

            data(ownid, 16);
        }

        const std::vector<uint8_t>& OwnId::data() const {
            return data_;
        }

        std::string OwnId::strdata() const {
            return strdata_;
        }

    }
}