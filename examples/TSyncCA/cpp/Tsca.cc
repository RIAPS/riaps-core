#include "include/Tsca.h"

namespace tsyncca {
   namespace components {
      
      Tsca::Tsca(_component_conf &config, riaps::Actor &actor) :
      TscaBase(config, actor), m_hasJoined(false), m_logcounter(0) {
          _logger->set_level(spd::level::info);
          _logger->set_pattern("%v");
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

              // If the component is not the leader, then proposeAction()
              if (GetLeaderId(gid)!="" && GetLeaderId(gid)!=GetCompUuid()) {
                  /**
                   * Agreeing to run the given action in the 2nd second;
                   */
                  timespec now;
                  clock_gettime(CLOCK_REALTIME, &now);

                  // TODO: make it random
                  now.tv_sec+=2;

                  /**
                   * Propose the action with id "0" to the leader. If accepted, it will be executed "now" (t+2secs).
                   */
                  std::string proposeId = ProposeAction(gid, "0", now);
              }
          }
      }

       void Tsca::ActionA(const uint64_t timerId) {
           /**
            * If the current scheduler is FIFO, increase the thread priority
            */

           sched_param schedParam;
           if (sched_getscheduler(getpid()) == SCHED_FIFO){
               sched_getparam(getpid(), &schedParam);
               sched_param newParam;
               newParam.__sched_priority = 95;
               sched_setparam(getpid(), &newParam);
           }

           /**
            * Busy wait. The action wakes up earlier (last param of ScheduleAction())
            * Use the high-precision WaitUntil() to reach the right time to fire the action.
            */
           WaitUntil(m_scheduled[timerId]);
           timespec tp;
           clock_gettime(CLOCK_REALTIME, &tp);


           m_logcounter++;
           _logger->info("{},{},semmi,semmi,{},{}", tp.tv_sec, tp.tv_nsec, m_scheduled[timerId].tv_sec, m_scheduled[timerId].tv_nsec);


           /**
            * The action is not scheduled anymore, remove the timerId.
            */
           m_scheduled.erase(timerId);

           /**
            * The action is not pending anymore.
            */
           m_pendingActions.erase("0");

           if (sched_getscheduler(getpid()) == SCHED_FIFO){
               sched_setparam(getpid(), &schedParam);
           }


           if (m_logcounter>200){
               std::exit(0);
           }
       }

       /**
        * The leader notifes the clients about the acceptance
        * @param groupId
        * @param proposeId
        * @param accepted
        */
       void Tsca::OnAnnounce(const riaps::groups::GroupId &groupId, const std::string &proposeId, bool accepted) {

           /**
            * If accepted and the propose is accepted previously on this node
            * And the same action is not scheduled now.
            */
           if (accepted &&
               m_accepted.find(proposeId)!=m_accepted.end() &&
               m_pendingActions.find("0")==m_pendingActions.end()) {

               // Get the time when the action must be performed
               timespec tp = m_accepted[proposeId];

               /**
                * Schedule the action, but the timer thread will wake up 2000microsec earlyer than the scheduled time.
                */
               auto timerId = ScheduleAction(tp, std::bind(&Tsca::ActionA, this, std::placeholders::_1), 2000*1000);
               m_scheduled[timerId] = tp;
               m_pendingActions.insert("0");
               m_accepted.erase(proposeId);
           }
       }

       /**
        * The leader sent a propose to the clients
        * @param groupId The group where the voting process happens
        * @param proposeId UniqueId of the propose. (was set by proposeAction())
        * @param actionId  The action to be executed
        * @param timePoint The time when the action must be started.
        */
       void Tsca::OnActionPropose(riaps::groups::GroupId &groupId,
                                  const std::string &proposeId,
                                  const std::string &actionId,
                                  const timespec& timePoint) {
           /**
            * Check that if there is any reason to not schedule the action.
            * Now all the proposed actions will be accepted.
            */
           if (false) {
               // There is a reason to not schedule the action
               SendVote(groupId, proposeId, false);
           }

           /**
            * Accepted propose, save the exact time for later.
            */
           m_accepted[proposeId] = timePoint;

           /**
            * Send the vote, accept.
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
