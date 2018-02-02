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

#ifdef CLOCK_NANOSLEEP
      void ST::OnClock(riaps::ports::PortBase *port) {
         if (!_experimentStarted){
             _experimentStarted = true;


             timespec wakeUpTime;
             clock_gettime(CLOCK_REALTIME, &wakeUpTime);
             wakeUpTime.tv_sec++;
             auto timerId = ScheduleAbsTimer(wakeUpTime);
             _cache[timerId] = wakeUpTime;

             _sLog[counter] = fmt::format("S[{}],{},{}", counter, wakeUpTime.tv_sec, wakeUpTime.tv_nsec);
             counter++;
             _logger->debug("S[{}]\t{}\t{}", timerId, wakeUpTime.tv_sec, wakeUpTime.tv_nsec );
         }
      }
#elif CHRONO_WAIT
        void ST::OnClock(riaps::ports::PortBase *port) {
            if (!_experimentStarted){
                _experimentStarted = true;


//             timespec wakeUpTime;
//             clock_gettime(CLOCK_REALTIME, &wakeUpTime);
//             wakeUpTime.tv_sec++;
//             auto timerId = ScheduleAbsTimer(wakeUpTime);
//             _cache[timerId] = wakeUpTime;

                auto now = std::chrono::system_clock::now();
                auto wakeUpTime = now + std::chrono::duration<int, std::milli>(1000);
                auto timerId = ScheduleAbsTimer(wakeUpTime);
                _chCache[counter] = wakeUpTime;
                //_sLog[counter] = fmt::format("S[{}],{},{}", counter, wakeUpTime.tv_sec, wakeUpTime.tv_nsec);
                counter++;
                //_logger->debug("S[{}]\t{}\t{}", timerId, wakeUpTime.tv_sec, wakeUpTime.tv_nsec );


            }
        }
#endif
      
      void ST::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }

       /**
        * Schedule clock_nanosleep() based timer
        * @param timerId
        */

#ifdef CLOCK_NANOSLEEP
       void ST::OnScheduledTimer(const uint64_t timerId) {
           timespec now;
           if (counter<200){

               clock_gettime(CLOCK_REALTIME, &now);
               _fLog[counter] = fmt::format("F[{}],{},{}", counter, now.tv_sec, now.tv_nsec);
                counter++;
           }

           if (counter<200){
               now.tv_sec++;
               auto tid = ScheduleAbsTimer(now);
               _cache[tid] = now;

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

       }
#elif CHRONO_WAIT
       void ST::OnScheduledTimer(const uint64_t timerId) {
           auto now = std::chrono::system_clock::now();
           if (counter<200){

               _chCache[counter] = now;
               counter++;
           }

           if (counter<200){

               auto wakeUpTime = now + std::chrono::duration<int, std::milli>(1000);
               auto timerId = ScheduleAbsTimer(wakeUpTime);
               _chCache[counter] = wakeUpTime;
               counter++;
           }

           if (counter>199) {
               _logger->info("Experiment ended");
               std::ofstream f("timers.csv");
               for (int i = 0; i < 200; i+=2) {
                   auto dur = duration_cast<std::chrono::nanoseconds>(_chCache[i+1] - _chCache[i]).count();
                   f<<dur<<std::endl;

               }
               _logger->info("Log saved");
               f.close();
               zclock_sleep(2000);
               std::terminate();
           }

       }
#endif
      
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
