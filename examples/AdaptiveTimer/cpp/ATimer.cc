/**
 * ScheduledTimer example
 *
 * Timer is scheduled by ScheduleAbsTimer(). In every second the OnScheduletimer() is called
 * and the timer is rescheduled for the next second.
 *
 * In the OnScheduleTimer() we measure the accuracy (delay), and in the rescheduling the offset value is optimized.
 *
 * The offset value specifies the early wakeup value of the timer.
 *
 * E.g.:
 *  ScheduleAbsTimer(time, OFFSET).
 *  The timer will fire at time-OFFSET and leaves enough room for the os/kernel/riapsApp to start the OnScheduleTimer().
 *
 *  WaitUntil() is called if the handler is fired much earlier (>100microsec).
 *
 *  If the timer is still in late, the OnScheduleTimer() increases the offset value (to the current max delay).
 *  If the timer is hurry (WaitUntil() runs for more than 100 microsec), then the offset value is decreased by
 *  the average of the delays. (we used exponential weighted average).
 */

#include "include/ATimer.h"

#define EARLY_WAKEUP_OFFSET_INIT 1000*1000 // 1000microsec in nanosec

namespace adaptivetimer {
   namespace components {
      
      ATimer::ATimer(_component_conf &config, riaps::Actor &actor) :
      ATimerBase(config, actor), _started(false), _earlyWakeupOffset(EARLY_WAKEUP_OFFSET_INIT), _avgDelay(EARLY_WAKEUP_OFFSET_INIT) {
          _logger->set_level(spd::level::info);
          _logger->set_pattern("%v");
      }
      
      void ATimer::OnClock(riaps::ports::PortBase *port) {
         if (!_started){
             _started = true;
             timespec now;
             now.tv_sec++;
             clock_gettime(CLOCK_REALTIME, &now);
             auto timerId = ScheduleAbsTimer(now, _earlyWakeupOffset); // 1000 microsec
             _timers[timerId] = now;
//             timerId = ScheduleAbsTimer(now, 1000*2000); // 2000 microsec
//             _timers[timerId] = now;
//             timerId = ScheduleAbsTimer(now, 1000*3000); // 3000 microsec
//             _timers[timerId] = now;
         }
      }
      
      void ATimer::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }


       void ATimer::OnScheduledTimer(const uint64_t timerId) {

           // When the handler is called
           timespec fireTime;
           clock_gettime(CLOCK_REALTIME, &fireTime);

           // When the handler should have been called
           timespec targetTime = _timers[timerId];
           WaitUntil(targetTime);

           // Current time after busy wait
           timespec now;
           clock_gettime(CLOCK_REALTIME, &now);

           // Schedule the new timer in the next second.
           now.tv_sec++;
           auto newTimerId = ScheduleAbsTimer(now, _earlyWakeupOffset);
           _timers[newTimerId] = now;
           now.tv_sec--;

           _logger->debug(fmt::format("[F],{},{}", fireTime.tv_sec, fireTime.tv_nsec));
           _logger->debug(fmt::format("[T],{},{}", targetTime.tv_sec, targetTime.tv_nsec));
           _logger->debug(fmt::format("[S],{},{}", now.tv_sec, now.tv_nsec));

           auto diffFromTarget = now - targetTime;
           auto diffInNano = diffFromTarget.tv_sec*BILLION + diffFromTarget.tv_nsec;

           // 100 microsec late => increase the offset (fire later)
           if (diffInNano>100*1000){
               auto oldOffset = _earlyWakeupOffset;
               _avgDelay = diffInNano;
               _earlyWakeupOffset+=_avgDelay;
               _logger->error("In late adjust offset: {}->{}", oldOffset, _earlyWakeupOffset);
           } else {
               // Too much wait between the firetime - targettime, the goal is to spend less time in WaitUntil()
               auto diffTargetFire = targetTime-fireTime;
               auto diffInNano2 = diffTargetFire.tv_sec*BILLION + diffTargetFire.tv_nsec;
               if (diffInNano2>100*1000){
                   // Calculate exp.weighted.avg
                   // TODO: bias correction?
                   if (_avgDelay = 0)
                       _avgDelay = diffInNano2;
                   else
                       _avgDelay = 0.9*(double)_avgDelay + 0.1*(double)diffInNano2;

                   // Decrease the offset
                   auto oldOffset = _earlyWakeupOffset;
                   if (_earlyWakeupOffset>_avgDelay) {
                       _earlyWakeupOffset = _earlyWakeupOffset - _avgDelay;
                       _logger->error("In hurry adjust offset: {}->{}", oldOffset, _earlyWakeupOffset);
                   }
               }
           }

           //_logger->info_if((diffFromTarget.tv_nsec/1000)<=100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           //_logger->error_if((diffFromTarget.tv_nsec/1000)>100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           _logger->info("Current offset is {}, avg delay is: {}, current delay is: {}", _earlyWakeupOffset, _avgDelay, diffFromTarget.tv_nsec);
       }

      
      ATimer::~ATimer() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new adaptivetimer::components::ATimer(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}
