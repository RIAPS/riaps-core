//
// Created by istvan on 12/4/19.
//

#ifndef RIAPS_CORE_R_GROUPID_H
#define RIAPS_CORE_R_GROUPID_H

#include <string>

#include <msgpack.hpp>


namespace riaps {
    namespace groups {
        /**
         * Uniquely identifies a group instance.
         */
        struct GroupId {
            /**
             * The group type, comes from the model file.
             */
            std::string group_type_id;

            /**
             * Name of the group, can be dynamic.
             */
            std::string group_name;

            bool operator<(const GroupId& other) const;
            bool operator==(const GroupId& other) const;

            ///\cond
            MSGPACK_DEFINE(group_name, group_type_id);
            ///\endcond
        };
    }
}
#endif //RIAPS_CORE_R_GROUPID_H
