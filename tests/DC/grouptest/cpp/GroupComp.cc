#include <GroupComp.h>

using namespace std;
using namespace riaps::groups;

namespace grouptest {
  namespace components {

    GroupComp::GroupComp(_component_conf &config, riaps::Actor &actor)
        : GroupCompBase(config, actor), m_joinedToA(false), m_joinedToB(false) {

      auto uid = zuuid_new();
      string uidStr = zuuid_str(uid);
      zuuid_destroy(&uid);

      m_publicGroupId = GroupId{GROUP_TYPE_GROUPA, "PublicGroup"};
      m_uniqueGroupId = GroupId{GROUP_TYPE_GROUPB, uidStr};
    }

    void GroupComp::OnClock(riaps::ports::PortBase *port) {
      if (!m_joinedToA) {
        _logger->info("Component joins to {}:{}", m_publicGroupId.groupTypeId, m_publicGroupId.groupName);
        auto joined = JoinToGroup(m_publicGroupId);
        if (joined){
          m_joinedToA = true;
        }
        _logger->error_if(!joined, "Couldn't join to group {}:{}", m_publicGroupId.groupTypeId, m_publicGroupId.groupName);
        
      } else {
        auto count = GetGroupMemberCount(m_publicGroupId);
        _logger->info("Number of members in groupA (including the current node): {}", count+1);
      }

      if (!m_joinedToB) {
        _logger->info("Component joins to {}:{}", m_uniqueGroupId.groupTypeId, m_uniqueGroupId.groupName);
        auto joined = JoinToGroup(m_uniqueGroupId);
        if (joined){
          m_joinedToB = true;
        }
        _logger->error_if(!joined, "Couldn't join to group {}:{}", m_uniqueGroupId.groupTypeId, m_uniqueGroupId.groupName);
      } else {
        auto count = GetGroupMemberCount(m_uniqueGroupId);
        _logger->info("Number of members in groupB (including the current node): {}", count+1);
      }
    }

    void GroupComp::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                   capnp::FlatArrayMessageReader &capnpreader,
                                   riaps::ports::PortBase *port) {}

    GroupComp::~GroupComp() {}
  } // namespace components
} // namespace grouptest

riaps::ComponentBase *create_component(_component_conf &config,
                                       riaps::Actor &actor) {
  auto result = new grouptest::components::GroupComp(config, actor);
  return result;
}

void destroy_component(riaps::ComponentBase *comp) { delete comp; }
