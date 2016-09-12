#include "actor.hpp"

int main() {

  zcm::Actor client_server_actor;
  client_server_actor.configure("configuration.json");
  client_server_actor.run();
  
  return 0;
}
