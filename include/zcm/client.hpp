/** @file    client.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Client class
 */

#ifndef CLIENT
#define CLIENT
#include <iostream>
#include <cerrno>
#include <zmq.hpp>

namespace zcm {

  /**
   * @brief Client class
   */
  class Client {
  public:

    /**
     * @brief Construct a client object
     * @param[in] name Client name
     * @param[in] ZMQ Context of the Actor Process
     * @param[in] timeout Client socket timeout
     */   
    Client(std::string name, zmq::context_t * actor_context, int timeout);

    /**
     * @brief Construct a client object with known endpoints
     * @param[in] name Client name
     * @param[in] ZMQ Context of the Actor Process
     * @param[in] endpoints A vector of endpoint strings
     * @param[in] timeout Client socket timeout
     */   
    Client(std::string name, zmq::context_t * actor_context, 
	   std::vector<std::string> endpoints, int timeout);

    /**
     * @brief Close the client ZMQ socket and destroy the context
     */    
    ~Client();

    /**
     * @brief Connect the client to a new set of endpoints
     * @param[in] new_endpoints New set of endpoints as a vector
     */ 
    void connect(std::vector<std::string> new_endpoints);

    /**
     * @brief Return the client name
     * @return Client name
     */    
    std::string get_name();

    /**
     * @brief Set timeout on the client to prevent endless blocking
     * @param[in] timeout New timeout value
     */
    void set_timeout(int timeout);

    /**
     * @brief Call the server
     * @param[in] message The message string. Serialize complex objects to strings with protobuf
     */  
    std::string call(std::string message);
  
  private:

    /** @brief Name of the publisher */  
    std::string name;

    /** @brief Vector of endpoints to connect to */  
    std::vector<std::string> endpoints;

    /** @brief ZMQ Context of the client */  
    zmq::context_t * context;

    /** @brief ZMQ Socket of the client */  
    zmq::socket_t * client_socket;

    /** @brief Timeout of the client socket */
    int client_socket_timeout;

  };

}

#endif
