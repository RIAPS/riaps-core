//
// Auto-generated by edu.vanderbilt.riaps.generator.ComponenetGenerator.xtend
//
#ifndef RIAPS_FW_ST_H
#define RIAPS_FW_ST_H

#include "STBase.h"

#define CLOCK_NANOSLEEP
//#define CHRONO_WAIT

namespace scheduledtimer {
   namespace components {
      
      class ST : public STBase {
         
         public:
         
         ST(_component_conf &config, riaps::Actor &actor);
         
         virtual void OnClock(riaps::ports::PortBase *port);
         
         void OnGroupMessage(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port);

          //void OnScheduledTimer(char* timerId, bool missed);
          void OnScheduledTimer(const uint64_t timerId);
         
         virtual ~ST();

      private:
          bool _experimentStarted;
          uint64_t counter;



#ifdef CLOCK_NANOSLEEP
           std::array<std::string, 300> _sLog;
           std::array<std::string, 300> _fLog;
           std::unordered_map<uint64_t, timespec> _cache;
#elif CHRONO_WAIT
           std::unordered_map<uint64_t, std::chrono::system_clock::time_point> _chCache;
#endif
      };
   }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_ST_H