/** @file    manager.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.07.21
 *  @brief   This file declares the Manager class
 */

#ifndef MANAGER
#define MANAGER
#include <iostream>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <thread>
#include "actor.hpp"

namespace zcm {

  /**
   * @brief Manager class
   */
  class Manager {
  public:

    /**
     * @brief Construct a manager object
     * @param[in] configuration_file Configuration file to prepare actor_list
     */
    Manager(std::string configuration_file);

    /**
     * @brief Kill all actors and destroy manager
     */
    ~Manager();

    /**
     * @brief Start an actor process
     * @param[in] Properties of the actor to start
     */
    void start_actor(Actor * actor);

    /**
     * @brief Deploy all actors in actor_list in separate child processes
     */
    void deploy();

  private:

    /** @brief A vector of actor objects */
    std::vector<Actor> actor_list;
  };

}
#endif
