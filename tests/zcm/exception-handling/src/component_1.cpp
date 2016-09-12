/** @file    component_1.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions of the Component_1 class
 */

#include "component_1.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load component_1.so
   */   
  extern "C" {
    Component* create_component() {
      return new Component_1();
    }
  }

  /**
   * @brief Construct component_1
   * Register all operations exposed by this component
   */    
  Component_1::Component_1() {
    register_functionality("timer_1_function",
			     std::bind(&Component_1::timer_1_function, this));
    register_functionality("subscriber_function",
			   std::bind(&Component_1::subscriber_function, 
				     this));
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */     
  void Component_1::timer_1_function() {
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> loop_iteration_random(500000 * 0.6, 500000);
    int loop_max = loop_iteration_random(rng);    
  
    // Business Logic for Timer_1_operation
    for(int i=0; i < loop_max; i++) {
      double result = 0.0;
      double x = 41865185131.214415;
      double y = 562056205.1515;
      result = x*y;
    }
    publisher("publisher_port")->send("Component_1");
    std::cout << "Component 1 : Timer : Published message: Component_1" << std::endl;    
  }

  /**
   * @brief A subscriber operation
   * This operation can be bound to a subscriber
   * Bind this operation to a subscriber in the JSON configuration
   */     
  void Component_1::subscriber_function() {
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> loop_iteration_random(700000 * 0.6, 700000);
    int loop_max = loop_iteration_random(rng);  
  
    // Business Logic for Name_Subscriber_operation
    for(int i=0; i < loop_max; i++) {
      double result = 0.0;
      double x = 41865185131.214415;
      double y = 562056205.1515;
      result = x*y;
    }
    std::string received_message = subscriber("Name_Subscriber")->message();
    std::cout << "Component 1 : Subscriber : Received message: " 
	      << received_message << std::endl;
  }    
    
}

