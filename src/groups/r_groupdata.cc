//
// Created by istvan on 12/16/19.
//

#include <groups/r_groupdata.h>
#include <spdlog/fmt/fmt.h>

namespace riaps {
    namespace groups {
        namespace data {
            GroupDataBase::GroupDataBase(char* data) {
                term_=0;
                for (auto i = 0; i <4; i++) {
                    term_ += data[i]*pow(255, 3-i);
                }
            }

            GroupDataBase::GroupDataBase(uint32_t term) {
                term_ = term;
            }

            const uint32_t GroupDataBase::term() const {
                return term_;
            }

            Authority::Authority(char* data) : GroupDataBase(data) {
                ldrid_.data(data + 4, 16);

                // Host, binary representation
                for (auto i = 20; i<24; i++) {
                    ldrhost_bin_.push_back(data[i]);
                    ldrhost_+= std::to_string(data[i]);
                    ldrhost_ += (i == 23) ? "" : ".";
                }

                uint8_t data_idx = 24;
                ldrport_=0;
                for (auto i = 0; i <4; i++) {
                    ldrport_ += data[data_idx++]*pow(255, 3-i);
                }
            }

            const OwnId & Authority::ldrid() const {
                return ldrid_;
            }

            const std::string & Authority::ldrhost() const {
                return ldrhost_;
            }

            const int Authority::ldrport() const {
                return ldrport_;
            }

            const std::string Authority::ldraddress_zmq() const {
                return fmt::format("tcp://{}:{}",ldrhost(), ldrport());
            }

            ReqVote::ReqVote(char *data) : GroupDataBase(data) {
                ownid_.data(data + 4, 16);
            }

            ReqVote::ReqVote(const riaps::groups::OwnId &ownid, uint32_t term) : GroupDataBase(term) {
                ownid_ = ownid;
            }

            const class riaps::groups::OwnId &ReqVote::ownid() const {
                return ownid_;
            }


        }
    }
}