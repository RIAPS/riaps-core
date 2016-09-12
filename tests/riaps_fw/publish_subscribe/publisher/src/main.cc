#include "../../../../../include/zcm/actor.hpp"

int main() {

  zcm::Actor publish_actor;
  publish_actor.configure("pub_conf.json");
  publish_actor.run();
  
  return 0;
}
