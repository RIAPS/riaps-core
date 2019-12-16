//
// Created by istvan on 12/16/19.
//

#ifndef RIAPS_CORE_R_OWNID_H
#define RIAPS_CORE_R_OWNID_H

#include <vector>
#include <string>
#include <functional>

namespace riaps {
    namespace groups {



        class OwnId {
        public:
            OwnId() = default;
            void data(char* data, size_t size);
            std::vector<char>& data();
            std::string strdata() const;
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
                if (obj1.strdata() == obj2.strdata())
                    return true;
                return false;
            }
        };
    }
}
#endif //RIAPS_CORE_R_OWNID_H
