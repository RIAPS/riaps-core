/** @file    component_2.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions of the Component_2 class
 */
#include "component_2.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load component_2.so
   */     
  extern "C" {
    Component* create_component() {
      return new Component_2();
    }
  }  

  /**
   * @brief Construct component_2
   * Register all operations exposed by this component
   */    
  Component_2::Component_2() {
    register_functionality("timer_function",
			   std::bind(&Component_2::timer_function, this));
    register_functionality("server_function",
			   std::bind(&Component_2::server_function,
				     this));
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */   
  void Component_2::timer_function() {
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> loop_iteration_random(800000 * 0.6, 800000);
    int loop_max = loop_iteration_random(rng);    
  
    // Business Logic for Timer_2_operation
    for(int i=0; i < loop_max; i++) {
      double result = 0.0;
      double x = 41865185131.214415;
      double y = 562056205.1515;
      result = x*y;
    } 
    publisher("publisher_port")->send("Component_2");
    std::cout << "Component 2 : Timer : Published message: Component_2" << std::endl;    
  }

  /**
   * @brief A server operation
   * This operation can be bound to a server and requested by some client
   * Bind this operation to a server in the JSON configuration
   */   
  void Component_2::server_function() {
    std::string request = server("server")->message();
    std::cout << "Component 2 : Server : Received request: " << request << std::endl;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> loop_iteration_random(600000 * 0.6, 600000);
    int loop_max = loop_iteration_random(rng);  
  
    // Business Logic for Service_Server_operation
    for(int i=0; i < loop_max; i++) {
      double result = 0.0;
      double x = 41865185131.214415;
      double y = 562056205.1515;
      result = x*y;
    }
    publisher("publisher_port")->send("Component_2");
    std::cout << "Component 2 : Server : Published message: Component_2" << std::endl;     
    server("server")->set_response("Component_2");
  }

}
