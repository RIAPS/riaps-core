/** @file    server_1.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Server_1 class
 */

#include "server_1.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load server.so
   */    
  extern "C" {
    Component* create_component() {
      return new Server_1();
    }
  }    

  /**
   * @brief Construct a server component
   * Register all operations exposed by this component
   */   
  Server_1::Server_1() {
    register_functionality("server_function",
			   std::bind(&Server_1::server_function, this));
  }

  /**
   * @brief A server operation
   * This operation can be requested by some client
   * Bind this operation to a server in the JSON configuration
   */    
  void Server_1::server_function() {
    std::string request = server("server_port")->message();

    std::cout << "Server_1 Operation : Received Request: " << request << std::endl;

    // Sleep for a few seconds - Fail a client or two to test exceptions
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    try {
      server("server_port")->set_response("ACK");
    }
    catch (std::runtime_error &e) {
      std::cout << "Server_1 Exception: " << e.what() << std::endl;
    }
  }  

}

