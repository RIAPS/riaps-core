#include "include/STimer.h"

namespace statictimer {
   namespace components {
      
      STimer::STimer(_component_conf &config, riaps::Actor &actor) :
      STimerBase(config, actor), _arrivedPings(0), _counter(0) {

      }
      
      void STimer::OnClock(riaps::ports::PortBase *port) {
          if (!_started){
              _started = true;
              timespec now;
              now.tv_sec++;
              clock_gettime(CLOCK_REALTIME, &now);
              auto timerId = ScheduleAbsTimer(now, 0);
              _timers[timerId] = now;
              _startTime = std::chrono::steady_clock::now();
          }
      }

       void STimer::OnPing(const Msg::Reader &message, riaps::ports::PortBase *port) {
           _arrivedPings++;
       }

       void STimer::OnScheduledTimer(const uint64_t timerId) {

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
           auto newTimerId = ScheduleAbsTimer(now, 0);
           _timers[newTimerId] = now;
           now.tv_sec--;

           _logger->debug(fmt::format("[F],{},{}", fireTime.tv_sec, fireTime.tv_nsec));
           _logger->debug(fmt::format("[T],{},{}", targetTime.tv_sec, targetTime.tv_nsec));
           _logger->debug(fmt::format("[S],{},{}", now.tv_sec, now.tv_nsec));

           auto diffFromTarget = now - targetTime;
           auto diffInNano = diffFromTarget.tv_sec*BILLION + diffFromTarget.tv_nsec;

           _logger->debug(fmt::format("[D],{},{}", diffFromTarget.tv_sec, diffFromTarget.tv_nsec));

           //_logger->info_if((diffInNano/1000)<=100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           //_logger->error_if((diffInNano/1000)>100,"Accuracy: {}s, {}ns", diffFromTarget.tv_sec, diffFromTarget.tv_nsec);
           //_logger->info("Current offset is {}, avg delay is: {}, current delay is: {}", _earlyWakeupOffset, _avgDelay, diffFromTarget.tv_nsec);

           _logs[_counter++] = fmt::format("{}",diffInNano);



           if (_counter>199) {
               _endTime=std::chrono::steady_clock::now();
               std::chrono::duration<double, std::milli> dur = _endTime-_startTime;

               double ratio = _arrivedPings/(dur.count()/1000.0);

               _logger->info("Experiment ended");
               std::ofstream f("timers.csv");
               for (int i = 0; i < 200; i++) {
                   f << _logs[i];
                   if (i==0)
                       f << ",Rate:," << ratio << std::endl;
                   else
                       f << std::endl;
               }
               _logger->info("Log saved");
               f.close();
               zclock_sleep(2000);
               std::terminate();
           }
       }
      
      void STimer::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      STimer::~STimer() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new statictimer::components::STimer(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}
