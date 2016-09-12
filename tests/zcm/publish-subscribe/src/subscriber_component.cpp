/** @file    subscriber_component.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Subscriber_Component class
 */

#include "subscriber_component.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load subscriber_component.so
   */    
  extern "C" {
    Component* create_component() {
      return new Subscriber_Component();
    }
  }    

  /**
   * @brief Construct a subscriber component
   * Register all operations exposed by this component
   */  
  Subscriber_Component::Subscriber_Component() {
    register_functionality("subscriber_function",
			   std::bind(&Subscriber_Component::subscriber_function, 
				     this));
  }

  /**
   * @brief A subscriber operation that can be bound to a subscriber port
   */   
  void Subscriber_Component::subscriber_function() {

    std::string received_message = subscriber("subscriber_port")->message();
    std::cout << "Subscriber Operation : Received Message: " 
	      << received_message << std::endl; 
  }

}
