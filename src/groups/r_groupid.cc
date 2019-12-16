//
// Created by istvan on 12/5/19.
//

#include <groups/r_groupid.h>

namespace riaps {
    namespace groups {
        bool GroupId::operator<(const GroupId &other) const {
            if (group_type_id == other.group_type_id){
                return group_name < other.group_name;
            }
            return group_type_id<other.group_type_id;
        }

        bool GroupId::operator==(const GroupId &other) const {
            return group_type_id == other.group_type_id && group_name == other.group_name;
        }
    }
}