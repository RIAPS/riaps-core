/** @file    actor.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Actor class
 */

#ifndef ACTOR
#define ACTOR
#include "json.hpp"
#include "component.hpp"
#include "zcm_endpoint.h"
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <thread>

namespace zcm {

  /**
   * @brief Actor class
   */
  class Actor {
  public:

    /**
     * @brief Default constructor
     */
    Actor() {}

    /**
     * @brief Construct an actor object
     * @param[in] actor_name Name of the actor process
     * @param[in] actor_configuration Name of the configuration file
     */
    Actor(std::string actor_name,
	  std::string actor_configuration) :
      name(actor_name),
      configuration(actor_configuration),
      pid(-1) {}

    /**
     * @brief Configure the component_instances vector
     * @param[in] configuration_file JSON configuration file to parse
     */
    void configure(std::string configuration_file);

    /**
     * @brief Spawn all component instances
     */     
    void run();

    /**
     * @brief Get actor name
     * @return Name of the actor
     */
    std::string get_name();

    /**
     * @brief Get actor configuration filename
     * @return Configuration filename
     */
    std::string get_configuration();

    /**
     * @brief Get actor PID
     * @return PID of the actor process
     */
    pid_t get_pid();

    /**
     * @brief Set actor PID
     * @param[in] new_pid PID of the actor process
     */
    void set_pid(pid_t new_pid);
    
  private:
    std::string name;
    std::string configuration;    
    pid_t pid;
    std::vector<Component*> component_instances;
  };

}

#endif
