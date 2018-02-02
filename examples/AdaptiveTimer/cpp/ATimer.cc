#include "include/ATimer.h"

#define EARLY_WAKEUP_OFFSET_INIT 1000*1000 // in nanosec

namespace adaptivetimer {
   namespace components {
      
      ATimer::ATimer(_component_conf &config, riaps::Actor &actor) :
      ATimerBase(config, actor), _started(false), _earlyWakeupOffset(EARLY_WAKEUP_OFFSET_INIT), _avgDelay(0) {
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
             //timerId = ScheduleAbsTimer(now, 1000*2000); // 2000 microsec
             //_timers[timerId] = now;
             //timerId = ScheduleAbsTimer(now, 1000*3000); // 3000 microsec
             //_timers[timerId] = now;
         }
      }
      
      void ATimer::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }

       void ATimer::OnScheduledTimer(const uint64_t timerId) {
           timespec fireTime;
           clock_gettime(CLOCK_REALTIME, &fireTime);

           timespec targetTime = _timers[timerId];
           WaitUntil(targetTime);

           timespec now;
           clock_gettime(CLOCK_REALTIME, &now);

           now.tv_sec++;
           auto newTimerId = ScheduleAbsTimer(now, _earlyWakeupOffset);
           _timers[newTimerId] = now;
           now.tv_sec--;

           _logger->debug(fmt::format("[F],{},{}", fireTime.tv_sec, fireTime.tv_nsec));
           _logger->debug(fmt::format("[T],{},{}", targetTime.tv_sec, targetTime.tv_nsec));
           _logger->debug(fmt::format("[S],{},{}", now.tv_sec, now.tv_nsec));

           auto diffFromTarget = now - targetTime;


           // If the event was fired too late, increase the _earlyWakeup, _avgDelay counts in with higher weight
           if (fireTime>now) {
               auto diffInNano = diffFromTarget.tv_sec*BILLION + diffFromTarget.tv_nsec;

               _avgDelay = _avgDelay*0.2+0.8*diffInNano;
               auto oldOffset = _earlyWakeupOffset;
               _earlyWakeupOffset+=_avgDelay;
               _logger->error("In late adjust offset: {}->{}", oldOffset, _earlyWakeupOffset);
           } else {
               // Check the time spent in WaitUntil();
               auto diffFireTarget = targetTime-fireTime;
               auto diffInNano = diffFireTarget.tv_sec*BILLION + diffFireTarget.tv_nsec;

               // Decrease the _earlyWakeup if the difference is greater than 100 microsec
               // Otherwise leave the _earlyWakeup
               if ((diffInNano/1000)>100){
                   _avgDelay = _avgDelay*0.9+0.1*diffInNano;
                   auto oldOffset = _earlyWakeupOffset;
                   _earlyWakeupOffset-=_avgDelay;
                   _logger->error("In hurry adjust offset: {}->{}", oldOffset, _earlyWakeupOffset);
               }

           }





           //_logger->info_if((diffFromTarget.tv_nsec/1000)<=100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           //_logger->error_if((diffFromTarget.tv_nsec/1000)>100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           _logger->info("Current offset is {}, avg delay is: {}, current delay is: {}", _earlyWakeupOffset, _avgDelay, diffFromTarget.tv_nsec);




//           _logs.push_back(fmt::format("[F],{},{}", fireTime.tv_sec, fireTime.tv_nsec));
//           _logs.push_back(fmt::format("[T],{},{}", targetTime.tv_sec, targetTime.tv_nsec));
//           _logs.push_back(fmt::format("[S],{},{}", --now.tv_sec, now.tv_nsec));

           if (_logs.size()>180) {
//               std::ofstream f("atimer.csv");
//               for (auto& s : _logs) {
//                   f<<s;
//               }
//               _logger->info("Log saved");
//               f.close();
//               zclock_sleep(2000);
               std::terminate();
           }
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
