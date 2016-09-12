/** @file    publisher.hpp 
 *  @author  Pranav Srinivas Kumar, Istvan Madari
 *  @date    2016.04.24
 *  @brief   This file declares the Publisher class
 */

#ifndef PUBLISHER
#define PUBLISHER
#include <iostream>
#include <zmq.hpp>

// Discovery service functions
#include "../../include/r_framework.h"
#include "zcm_endpoint.h"
#include "servicebase.h"

namespace zcm {

  /**
   * @brief Publisher class
   */
  class Publisher : public servicebase {
  public:

    /**
     * @brief Construct a publisher object
     * @param[in] name Publisher name
     * @param[in] ZMQ Context of the Actor Process
     */  
    Publisher(std::string name, zmq::context_t * actor_context);

    /**
     * @brief Construct a publisher object with known endpoints
     * @param[in] name Publisher name
     * @param[in] ZMQ Context of the Actor Process
     * @param[in] endpoints A vector of endpoint strings
     */  
    Publisher(std::string name, zmq::context_t * actor_context, 
	      std::vector<zcm_endpoint> endpoints);

    /**
     * @brief Close the publisher ZMQ socket and destroy the context
     */  
    ~Publisher();

    /**
     * @brief Bind the publisher to a new set of endpoints
     * @param[in] new_endpoints New set of endpoints as a vector
     */    
    void bind(std::vector<zcm_endpoint> new_endpoints);



    /**
     * @brief Add a new endpoint to the publisher
     * @param[in] new_connection New endpoint to bind to
     */    
    void add_connection(std::string new_connection);

    /**
     * @brief Publish a new message
     * @param[in] message The message string. Serialize complex objects to strings with protobuf
     */
    void send(std::string message);

  private:

    /** @brief Name of the publisher */
    //std::string name;

    /** @brief ZMQ Context of the publisher */
    zmq::context_t * context;

    /** @brief ZMQ Socket of the publisher */
    zmq::socket_t * publisher_socket;

    /** @brief Vector of endpoints to bind to */
    std::vector<zcm_endpoint> endpoints;
  };

}

#endif
