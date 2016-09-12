/** @file    client_component.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Client_Component class
 */

#include "client_component.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load client_component.so
   */    
  extern "C" {
    Component* create_component() {
      return new Client_Component();
    }
  }  

  /**
   * @brief Construct a client component
   * Register all operations exposed by this component
   */     
  Client_Component::Client_Component() {
    register_functionality("timer_function",
			   std::bind(&Client_Component::timer_function, this));
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */   
  void Client_Component::timer_function() {
    // Create new message
    TestMessage new_message;
    new_message.set_message("client_timer_message");
    new_message.set_id(0);

    // Prepare request string
    std::string * request_string = new std::string;
    new_message.SerializeToString(request_string);

    // Call server
    std::string response = client("client_port")->call(*request_string);
    std::cout << "Client Timer : Received response: " << response << std::endl;
  }  

}
