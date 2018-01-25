#include <ST.h>

#define TIMER_RATE 1000

namespace scheduledtimer {
   namespace components {
      
      ST::ST(_component_conf &config, riaps::Actor &actor) :
      STBase(config, actor) {
          _experimentStarted = false;
          _logger->set_level(spd::level::debug);
          _logger->set_pattern("%v");
          counter=0;
      }
      
      void ST::OnClock(riaps::ports::PortBase *port) {
         if (!_experimentStarted){
             _experimentStarted = true;

             timespec wakeUpTime;
             clock_gettime(CLOCK_REALTIME, &wakeUpTime);
             wakeUpTime.tv_sec++;
             auto timerId = ScheduleAbsTimer(wakeUpTime);

             _sLog[counter] = fmt::format("S[{}],{},{}", counter, wakeUpTime.tv_sec, wakeUpTime.tv_nsec);
             counter++;
             //_logger->debug("S[{}]\t{}\t{}", timerId, wakeUpTime.tv_sec, wakeUpTime.tv_nsec );


         }
      }
      
      void ST::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }

       //void ST::OnScheduledTimer(const uint64_t, bool missed) {
       void ST::OnScheduledTimer(char*, bool missed) {
           timespec now;
           if (counter<200){

               clock_gettime(CLOCK_REALTIME, &now);
               _fLog[counter] = fmt::format("F[{}],{},{}", counter, now.tv_sec, now.tv_nsec);
                counter++;
           }

           if (counter<200){
               now.tv_sec++;
               auto tid = ScheduleAbsTimer(now);

               _sLog[counter] = fmt::format("S[{}],{},{}", counter, now.tv_sec, now.tv_nsec);
                counter++;
           }

           if (counter>199) {
               _logger->info("Experiment ended");
               std::ofstream f("timers.csv");
               for (int i = 0; i < 200; i++) {
                   f<<_sLog[i];
                   f<<std::endl;
                   f<<_fLog[i];
                   f<<std::endl;
               }
               _logger->info("Log saved");
               f.close();
               zclock_sleep(2000);
               std::terminate();
           }



           //  timespec now;
//           clock_gettime(CLOCK_REALTIME, &now);
//           _logger->debug("F[{}]\t{}\t{}", timerId, now.tv_sec, now.tv_nsec );
//
//           now.tv_sec++;
//           auto tid = ScheduleAbsTimer(now);
//           _logger->debug("S[{}]\t{}\t{}", tid, now.tv_sec, now.tv_nsec );
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
