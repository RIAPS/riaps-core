/** @file    server_component.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Server_Component class
 */

#include "server_component.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load server_component.so
   */    
  extern "C" {
    Component* create_component() {
      return new Server_Component();
    }
  }    

  /**
   * @brief Construct a server component
   * Register all operations exposed by this component
   */   
  Server_Component::Server_Component() {
    register_functionality("server_function",
			   std::bind(&Server_Component::server_function, this));
  }

  /**
   * @brief A server operation
   * This operation can be requested by some client
   * Bind this operation to a server in the JSON configuration
   */    
  void Server_Component::server_function() {
    std::string request = server("server_port")->message();
    TestMessage received_request_obj;
    received_request_obj.ParseFromString(request);
    std::cout << "Server Operation : Received Request: " 
	      << received_request_obj.message() 
	      << " ID : " << received_request_obj.id() << std::endl;
    server("server_port")->set_response("ACK");
  }  

}

