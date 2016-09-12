#include "actor.hpp"

int main() {

  zcm::Actor three_component_actor;
  three_component_actor.configure("configuration.json");
  three_component_actor.run();
  
  return 0;
}
