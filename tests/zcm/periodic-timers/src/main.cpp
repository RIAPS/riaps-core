#include "actor.hpp"

int main() {

  zcm::Actor periodic_timers_actor;
  periodic_timers_actor.configure("configuration.json");
  periodic_timers_actor.run();
  
  return 0;
}

