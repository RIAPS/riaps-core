#ifndef COMMON_H
#define COMMON_H

#include "componentmodel/r_componentbase.h"

#define GROUP_TYPE_GROUPA "groupA"
#define GROUP_TYPE_GROUPB "groupB"

static riaps::groups::GroupId groupIdA{GROUP_TYPE_GROUPA, "A"};
static riaps::groups::GroupId groupIdB{GROUP_TYPE_GROUPB, "B"};

void logGroupMessage(std::shared_ptr<spd::logger> logger, const std::string& functionName, const riaps::groups::GroupId& groupId, const std::string& sourceComp) {
    logger->info("::{} from ({},{}) <- {}",
                 functionName,
                  groupId.groupName,
                  groupId.groupTypeId,
                  sourceComp);
};

#endif