/** @file    component_3.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions of the Component_3 class
 */

#include "component_3.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load component_3.so
   */     
  extern "C" {
    Component* create_component() {
      return new Component_3();
    }
  }  

  /**
   * @brief Construct component_3
   * Register all operations exposed by this component
   */   
  Component_3::Component_3() {
    register_functionality("timer_function", std::bind(&Component_3::timer_function, this));
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */    
  void Component_3::timer_function() {
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> loop_iteration_random(500000 * 0.6, 500000);
    int loop_max = loop_iteration_random(rng);  
  
    // Business Logic for Timer_3_operation
    for(int i=0; i < loop_max; i++) {
      double result = 0.0;
      double x = 41865185131.214415;
      double y = 562056205.1515;
      result = x*y;
    }
    
    std::string response;
    try {
      response = client("client_port")->call("Component_3");    
    }
    catch (std::runtime_error &e) {
      std::cout << "Client Call Exception: " << e.what() << std::endl;
    }
    std::cout << "Component 3 : Timer : Called Component_2::Server : " << "Received: " << response << std::endl;
  }  
}
