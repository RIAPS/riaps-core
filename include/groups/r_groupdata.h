//
// Created by istvan on 12/16/19.
//

#ifndef RIAPS_CORE_R_GROUPDATA_H
#define RIAPS_CORE_R_GROUPDATA_H

#include <spdlog_setup/conf.h>
#include "r_ownid.h"

namespace spd = spdlog;

namespace riaps {
    namespace groups {
        namespace data {

            class GroupDataBase {
            public:
                GroupDataBase(uint8_t *data);
                GroupDataBase(uint32_t term);
                const uint32_t term() const;
                virtual std::vector<uint8_t> to_bytes() const = 0;

                /**
                 * Bytes to unsigned long (uint32_t)
                 * @return
                 */
                uint32_t btoul(uint8_t* bytes, size_t len);

                /**
                 * Unsigned long into bytes
                 * @param number The number to be converted
                 * @param len Number of bytes
                 * @return Byte representation of number
                 */
                std::vector<uint8_t> ultob(uint32_t number, size_t len) const;
                void ultob(uint32_t number, size_t len, std::vector<uint8_t>& result, int start_idx) const;

                std::shared_ptr<spd::logger> logger_;

            protected:
                uint32_t term_;
            };

            class ReqVote : public GroupDataBase{
            public:
                ReqVote(uint8_t *data);
                ReqVote(const OwnId &ownid, uint32_t term);
                const OwnId &ownid() const;
                virtual std::vector<uint8_t> to_bytes() const;
            private:
                OwnId ownid_;
            };

            class RspVote : public GroupDataBase {
            public:
                RspVote(uint8_t *data);
                RspVote(uint32_t term, const OwnId& vote_for, const OwnId& vote_by, bool vote);
                const OwnId &vote_for() const;
                const OwnId &voted_by() const;
                bool vote() const;
                virtual std::vector<uint8_t> to_bytes() const;
            private:
                OwnId vote_for_;
                bool vote_;
                OwnId voted_by_;
            };

            class Authority : public GroupDataBase {
            public:
                Authority(uint8_t* data);
                Authority(uint32_t term, OwnId leader, std::string host, int port);
                const OwnId &ldrid() const;
                const std::string& ldrhost() const;
                const int ldrport() const;
                const std::string ldraddress_zmq() const;
                virtual std::vector<uint8_t> to_bytes() const;
            private:
                OwnId ldrid_;
                std::string ldrhost_;
                std::vector<uint8_t> ldrhost_bin_;
                int ldrport_;
            };
        }

    }
}


#endif //RIAPS_CORE_R_GROUPDATA_H
