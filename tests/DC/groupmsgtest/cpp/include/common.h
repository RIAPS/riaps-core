#ifndef COMMON_H
#define COMMON_H

#include "componentmodel/r_componentbase.h"

#define GROUP_TYPE_GROUPA "groupA"
#define GROUP_TYPE_GROUPB "groupB"

static riaps::groups::GroupId groupIdA{GROUP_TYPE_GROUPA, "GroupA Instance"};
static riaps::groups::GroupId groupIdB{GROUP_TYPE_GROUPB, "GroupB Instance"};

#endif