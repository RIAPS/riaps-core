#include "include/Tsca.h"

namespace tsyncca {
   namespace components {
      
      Tsca::Tsca(_component_conf &config, riaps::Actor &actor) :
      TscaBase(config, actor), m_hasJoined(false) {
          m_actions["0"] = std::bind(&Tsca::ActionA, this);
          _logger->set_level(spd::level::debug);
      }
      
      void Tsca::OnClock(riaps::ports::PortBase *port) {
          if (!m_hasJoined) {
              bool rc = this->JoinToGroup({GROUP_TYPE_GROUPA, "Korte"});
              if (rc) {
                  _logger->debug("Successfully joined to group");
                  m_hasJoined = true;
              } else {
                  _logger->debug("Failed to join");
              }
          } else {
              riaps::groups::GroupId gid{GROUP_TYPE_GROUPA, "Korte"};


              // If the component is not the leader, then propose()
              if (GetLeaderId(gid)!="" && GetLeaderId(gid)!=GetCompUuid()) {
                  // The component already joined, read the file and lets vote about the content of the file
                  std::ifstream f;
                  f.open("dcoordvote.txt");
                  std::string line;
                  f >> line;

                  /**
                   * Agreeing to run the given action in the next second;
                   */
                  timespec now;
                  clock_gettime(CLOCK_REALTIME, &now);

                  // TODO: make it random
                  now.tv_sec++;

                  std::string proposeId = ProposeAction(gid, "0", now);
                  std::string leaderId = GetLeaderId(gid);
              }
          }
      }

       void Tsca::ActionA() {
           _logger->info("ActionA is called");
       }

       void Tsca::OnAnnounce(const riaps::groups::GroupId &groupId, const std::string &proposeId, bool accepted) {
           _logger->info("Propose {} is {}", proposeId, accepted?"accepted":"rejected");

           if (accepted && m_accepted.find(proposeId)!=m_accepted.end()) {
               ScheduleAbsTimer(m_accepted[proposeId], 1000);
           }
       }

       void Tsca::OnActionPropose(riaps::groups::GroupId &groupId,
                                  const std::string &proposeId,
                                  const std::string &actionId,
                                  const timespec& timePoint) {
           _logger->info("Action {} is proposed by the leader", actionId);

           /**
            * Check that there is no reason to not schedule the action
            */
           if (false) {
               // There is a reason to not schedule the action
               SendVote(groupId, proposeId, false);
           }

           m_accepted[proposeId] = timePoint;

           /**
            * Send the vote
            */
            SendVote(groupId, proposeId, true);
       }
      
      void Tsca::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      Tsca::~Tsca() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new tsyncca::components::Tsca(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}
