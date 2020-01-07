//
// Created by istvan on 12/16/19.
//

#ifndef RIAPS_CORE_R_OWNID_H
#define RIAPS_CORE_R_OWNID_H

#include <vector>
#include <string>
#include <functional>

#include <spdlog/logger.h>

namespace riaps {
    namespace groups {
        class OwnId {
        public:
            OwnId() = default;
            //OwnId(const OwnId& rhs);
            //OwnId operator=(const OwnId& rhs);
            void data(const std::vector<uint8_t> & actorid, uint64_t group_address);
            void data(char* data, size_t size);
            const std::vector<char>& data() const;
            std::string strdata() const;
            bool operator==(const OwnId& other) const {
                return strdata() == other.strdata();
            }
            bool operator!=(const OwnId& other) const {
                return strdata() != other.strdata();
            }
            ~OwnId() = default;
        private:
            std::vector<char> data_;
            std::string strdata_;
        };

        struct OwnIdHasher {
            size_t operator()(const OwnId& obj) const {
                return std::hash<std::string>{}(obj.strdata());
            }
        };

        struct OwnIdComparator {
            bool operator()(const OwnId& obj1, const OwnId& obj2) const
            {
                if (obj1 == obj2)
                    return true;
                return false;
            }
        };
    }
}
#endif //RIAPS_CORE_R_OWNID_H
