#include <ST.h>

#define TIMER_RATE 1000

namespace scheduledtimer {
   namespace components {
      
      ST::ST(_component_conf &config, riaps::Actor &actor) :
      STBase(config, actor) {
          _experimentStarted = false;
          _logger->set_level(spd::level::debug);
      }
      
      void ST::OnClock(riaps::ports::PortBase *port) {
         if (_experimentStarted){
             _experimentStarted = true;

             auto wakeUpTime = std::chrono::steady_clock::now();
             wakeUpTime+=duration<int, std::milli>(TIMER_RATE);
             auto timerId = ScheduleTimer(wakeUpTime);
             _logger->debug("S[{}];{}", timerId,  duration_cast<std::chrono::microseconds>(wakeUpTime.time_since_epoch()).count());
         }
      }
      
      void ST::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }

       void ST::OnScheduledTimer(const uint64_t timerId, bool missed) {
           auto now = std::chrono::steady_clock::now();
            _logger->debug("F[{}];{}",timerId, duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());

           auto wakeUpTime = now + duration<int, std::milli>(TIMER_RATE);
           auto newTimerId = ScheduleTimer(wakeUpTime);
           _logger->debug("S[{}];{}", newTimerId,  duration_cast<std::chrono::microseconds>(wakeUpTime.time_since_epoch()).count());
       }
      
      ST::~ST() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new scheduledtimer::components::ST(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}
