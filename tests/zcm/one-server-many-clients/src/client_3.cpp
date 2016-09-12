/** @file    client_3.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Client_3 class
 */

#include "client_3.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load client.so
   */    
  extern "C" {
    Component* create_component() {
      return new Client_3();
    }
  }  

  /**
   * @brief Construct a client component
   * Register all operations exposed by this component
   */     
  Client_3::Client_3() {
    register_functionality("call_the_server",
			   std::bind(&Client_3::call_the_server, this));
  }

  /**
   * @brief Call the server
   * This operation is used to call the server!
   * Bind this operation to a periodic timer in the JSON configuration
   */   
  void Client_3::call_the_server() {

    // Create new message
    std::string new_message = "Client_3 calling...";

    // Call server
    try {
      std::string response = client("client_port")->call(new_message);
      std::cout << "Client_3 : Received response: " << response << std::endl;
    }
    catch (std::runtime_error &e) {
      std::cout << "Client_3 Exception: " << e.what() << std::endl;
    }
  }  

}
