/** @file    manager.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.07.21
 *  @brief   This file contains definitions for the Manager class
 */

#include "../../include/zcm/manager.hpp"

namespace zcm {

  // Construct a manager object from configuration file
  Manager::Manager(std::string configuration_file) {
      Json::Value root;
      std::ifstream configuration(configuration_file, std::ifstream::binary);
      configuration >> root;
      
      for (unsigned int i = 0; i < root["Actors"].size(); i++) {
	std::string actor_name = root["Actors"][i]["Name"].asString();
	std::string actor_config = root["Actors"][i]["Configuration"].asString();
	Actor new_actor(actor_name, actor_config);
	actor_list.push_back(new_actor);
      }

    }

  // Kill all actors being managed
  Manager::~Manager() {
    for (auto & actor : actor_list) {
      if(actor.get_pid() > 0) {
	kill(actor.get_pid(), SIGKILL);
      }
    }
  }

  // Start an actor process
  void Manager::start_actor(Actor * actor) {

    std::cout << "Starting actor: " << actor->get_name() << std::endl;

    pid_t pid = fork(); /* Create a child process */
    
    switch (pid) {
    case -1: /* Error */
      std::cerr << "Error : fork() failed.\n";
      exit(1);
    case 0: /* Child process */
      actor->set_pid(pid);
      execl("./actor", "actor", "--config", actor->get_configuration().c_str(), "&", NULL); 
      std::cerr << "Error : execl() failed!\n"; 
      exit(1);
    default: /* Parent process */      
      std::cout << "Process created with pid " << pid << "\n";
      int status;
      
      while (!WIFEXITED(status)) {
	waitpid(pid, &status, 0); /* Wait for the process to complete */
      }
      
      std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";
    }
  }

  // Deploy all actors in actor_list
  void Manager::deploy() {
    std::vector<std::thread *> actor_threads;
    for (auto & actor : actor_list) {
      std::cout << "Deploying actor: " << actor.get_name() << std::endl;
      std::thread * actor_thread = new std::thread(&Manager::start_actor, this, &actor);
      actor_threads.push_back(actor_thread);
      // actor_thread->detach();
    } 

    for (auto & actor_thread : actor_threads)
      actor_thread->join();
  }

}
