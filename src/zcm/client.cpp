/** @file    client.cpp 
 *  @author  Pranav Srinivas Kumar, Istvan Madari
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Client class
 */

#include "../../include/zcm/client.hpp"

namespace zcm {

  // Construct a simple client
  Client::Client(std::string name, 
		 zmq::context_t * actor_context,
		 int timeout = 500) : 
    name(name), 
    context(actor_context),
    client_socket_timeout(timeout) {}

  // Construct a client with known endpoints
  Client::Client(std::string name, 
		 zmq::context_t * actor_context,
		 std::vector<std::string> endpoints, 
		 int timeout = 500) :
    name(name),
    endpoints(endpoints),
    client_socket_timeout(timeout) {
    context = actor_context;
    //context = new zmq::context_t(1);
    client_socket = new zmq::socket_t(*context, ZMQ_REQ);
    client_socket->setsockopt(ZMQ_RCVTIMEO, client_socket_timeout); // milliseconds

    for (auto endpoint : endpoints) {
      try {
	client_socket->connect(endpoint);
      } catch (zmq::error_t error) {
	std::cout << "Unable to connect client " << 
	  name << " to " << endpoint << std::endl;      
      }
    }
  }

  // Destroy the client
  Client::~Client() {
    client_socket->close();
    delete context;
    delete client_socket;
  }

  void Client::connect(std::vector<std::string> new_endpoints) {
    endpoints = new_endpoints;
    // context = new zmq::context_t(1);
    client_socket = new zmq::socket_t(*context, ZMQ_REQ);
    client_socket->setsockopt(ZMQ_RCVTIMEO, client_socket_timeout); // milliseconds
    for (auto endpoint : endpoints) {
      try {
	client_socket->connect(endpoint);
      } catch (zmq::error_t error) {
	std::cout << "Unable to connect client " << 
	  name << " to " << endpoint << std::endl;      
      }
    }
  }  

  std::string Client::get_name() {
    return name;
  }

  void Client::set_timeout(int timeout) {
    client_socket_timeout = timeout;
    if (client_socket != NULL)
      client_socket->setsockopt(ZMQ_RCVTIMEO, client_socket_timeout);
  }

  std::string Client::call(std::string message) {
    zmq::message_t request(message.length());
    memcpy(request.data(), message.c_str(), message.length());
    try {
      client_socket->send(request);  
    }
    catch (zmq::error_t &e) {
      throw std::runtime_error(e.what());
    }
    // Wait for response
    zmq::message_t reply;
    client_socket->recv(&reply);
    return std::string(static_cast<char*>(reply.data()), reply.size());
  }

}
