/** @file    operation_types.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for various Operation Types
 */

#include "../../include/zcm/operation_types.hpp"

namespace zcm {

  // Return the operation name
  std::string Base_Operation::get_name() {
    return name;
  }

  // Return the operation priority
  unsigned int Base_Operation::get_priority() const {
    return priority;
  }

  // Execute a timer operation
  void Timer_Operation::execute() {
    operation_function();
  }

  // Execute a subsciber operation
  void Subscriber_Operation::execute() {
    operation_function();
  }

  // Execute the server operation and respond to the client
  void Server_Operation::execute() {
    operation_function();
    zmq::message_t reply(response_ptr->length());
    memcpy(reply.data(), response_ptr->c_str(), response_ptr->length());
    try {
      socket_ptr->send(reply);
    }
    catch (zmq::error_t &e) {
      throw std::runtime_error(e.what());
    }
    set_ready();
  }

  // Return the ZMQ server socket pointer
  zmq::socket_t * Server_Operation::get_socket_ptr() {
    return socket_ptr;
  }

  std::string * Server_Operation::get_response_ptr() {
    return response_ptr;
  }

  // Set the server as "ready" to receive new request from client
  void Server_Operation::set_ready() {
    *recv_ready = true;
  }

}
