#include "actor.hpp"

int main() {

  zcm::Actor publish_subscribe_actor;
  publish_subscribe_actor.configure("configuration.json");
  publish_subscribe_actor.run();
  
  return 0;
}
