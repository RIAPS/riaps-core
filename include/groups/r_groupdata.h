//
// Created by istvan on 12/16/19.
//

#ifndef RIAPS_CORE_R_GROUPDATA_H
#define RIAPS_CORE_R_GROUPDATA_H

#include "r_ownid.h"

namespace riaps {
    namespace groups {
        namespace data {

            class GroupDataBase {
            public:
                GroupDataBase(char *data);
                GroupDataBase(uint32_t term);
                const uint32_t term() const;
            private:
                uint32_t term_;
            };

            class ReqVote : public GroupDataBase{
            public:
                ReqVote(char *data);
                ReqVote(const OwnId &ownid, uint32_t term);
                const OwnId &ownid() const;
            private:
                OwnId ownid_;
            };

            class Authority : public GroupDataBase {
            public:
                Authority(char* data);
                const OwnId &ldrid() const;
                const std::string& ldrhost() const;
                const int ldrport() const;
                const std::string ldraddress_zmq() const;
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
