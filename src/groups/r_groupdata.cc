//
// Created by istvan on 12/16/19.
//

#include <groups/r_groupdata.h>
#include <spdlog/fmt/fmt.h>

using namespace std;

namespace riaps {
    namespace groups {
        namespace data {
            /**
             * Common functions for leader election datatypes.
             */

            GroupDataBase::GroupDataBase(uint8_t* data) {
                term_= btoul(data, 4);
            }

            GroupDataBase::GroupDataBase(uint32_t term) {
                term_ = term;
            }

            const uint32_t GroupDataBase::term() const {
                return term_;
            }

            uint32_t GroupDataBase::btoul(uint8_t *bytes, size_t len) {
                uint32_t result = 0;
                for (auto i = 0; i <len; i++) {
                    result += bytes[i]*pow(256, len-1-i);
                }
                return result;
            }

            std::vector<uint8_t> GroupDataBase::ultob(uint32_t number, size_t len) const {
                vector<uint8_t> result(len, 0);

                int idx = len-1;
                for (int i = 0; i<len; i++) {
                    result[idx--] = number%256;
                    number = number/256;
                }

                return result;
            }

            void GroupDataBase::ultob(uint32_t number, size_t len, std::vector<uint8_t> &result, int start_idx) const {
                auto tmp = ultob(number, len);
                auto tmp_idx = 0;
                for (int i = start_idx; i<len; i++) {
                    result[i] = tmp[tmp_idx++];
                }
            }

            /**
             * Authority - Broadcasting the leader host/port.
             * Leader election
             */

            Authority::Authority(uint8_t* data) : GroupDataBase(data) {
                ldrid_.data(data + 4, 16);

                // Host, binary representation
                for (auto i = 20; i<24; i++) {
                    ldrhost_bin_.push_back(data[i]);
                    ldrhost_+= std::to_string(data[i]);
                    ldrhost_ += (i == 23) ? "" : ".";
                }

                ldrport_ = btoul(data+24,4);
            }

            Authority::Authority(uint32_t term, riaps::groups::OwnId leader, std::string host, int port) :
                GroupDataBase(term),
                ldrport_(port),
                ldrhost_(host),
                ldrid_(leader) {
                string num = "";
                for (int i = 0; i<host.length(); i++) {
                    if (host[i] == '.') {
                        ldrhost_bin_.push_back(stoi(num));
                        num = "";
                    } else {
                        num+=host[i];
                    }
                }
                ldrhost_bin_.push_back(stoul(num));
            }

            //(_term,_leader,_host,_port)
            std::vector<uint8_t> Authority::to_bytes() const {
                // Authority message, represented in btes
                vector<uint8_t> result(28, 0);

                // Copy the term (4bytes)
                ultob(this->term(), 4, result, 0);

                // Copy the leaderid (16bytes)
                uint8_t target_idx = 4;
                for (uint8_t d : ldrid_.data()) {
                    result[target_idx++] = d;
                }

                // Copy the host (4bytes)
                for (uint8_t d : ldrhost_bin_) {
                    result[target_idx++] = d;
                }

                // Convert port into binary and copy int the result (4bytes)
                ultob(this->ldrport_, 4, result, 24);

                return result;
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

            /**
             * ReqVote - Request for vote
             * Leader election
             */

            ReqVote::ReqVote(uint8_t *data) : GroupDataBase(data) {
                ownid_.data(data + 4, 16);
            }

            ReqVote::ReqVote(const riaps::groups::OwnId &ownid, uint32_t term) : GroupDataBase(term) {
                ownid_ = ownid;
            }

            std::vector<uint8_t> ReqVote::to_bytes() const {
                vector<uint8_t> rsp_bytes(20, 0);
                ultob(term(), 4, rsp_bytes, 0);
                for (int i=4;i<20;i++){
                    rsp_bytes[i] = this->ownid_.data()[i-4];
                }
                return rsp_bytes;
            }

            const class riaps::groups::OwnId &ReqVote::ownid() const {
                return ownid_;
            }

            /**
             * RspVote - Response to vote request
             * Leader election
             */

            RspVote::RspVote(uint32_t term, const riaps::groups::OwnId &vote_for, const riaps::groups::OwnId &vote_by,
                             bool vote) : GroupDataBase(term), vote_for_(vote_for), voted_by_(vote_by), vote_(vote) {

            }

            std::vector<uint8_t> RspVote::to_bytes() const{
                vector<uint8_t> rsp_bytes(40, 0);
                ultob(term(), 4, rsp_bytes, 0);

                for (int i=4;i<20;i++){
                    rsp_bytes[i] = vote_for_.data()[i-4];
                }

                rsp_bytes[23] = vote_;
                for (int i=24;i<40;i++) {
                    rsp_bytes[i] = this->voted_by_.data()[i-24];
                }
                return rsp_bytes;
            }

            RspVote::RspVote(uint8_t *data) : GroupDataBase(data) {
                vote_for_.data(data+4, 16);
                vote_ = data[23];
                voted_by_.data(data+24, 16);
            }

            const OwnId & RspVote::vote_for() const {
                return vote_for_;
            }

            const OwnId & RspVote::voted_by() const {
                return voted_by_;
            }
        }
    }
}