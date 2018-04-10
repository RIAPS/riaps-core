#ifndef COMMON_H
#define COMMON_H

#include "componentmodel/r_componentbase.h"

#define GROUP_TYPE_GROUPA "groupA"

static riaps::groups::GroupId groupIdA{GROUP_TYPE_GROUPA, "A"};

void logGroupMessage(std::shared_ptr<spd::logger> logger, const std::string& functionName, const riaps::groups::GroupId& groupId, const std::string& sourceComp) {
    logger->info("::{} from ({},{}) <- {}",
                 functionName,
                  groupId.groupName,
                  groupId.groupTypeId,
                  sourceComp);
};

#endif