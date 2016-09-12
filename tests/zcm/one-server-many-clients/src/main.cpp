#include "actor.hpp"

int main(int argc, char *argv[]) {

  std::string config_filename("");
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--config")) 
      config_filename = argv[i+1]; 
  }
  zcm::Actor zcm_actor;
  zcm_actor.configure(config_filename);
  zcm_actor.run();
  return 0;
}
