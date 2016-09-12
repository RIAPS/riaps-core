/** @file    operation_types.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares Operation Types
 */

#ifndef OPERATION_TYPES
#define OPERATION_TYPES
#include <iostream>
#include <functional>
#include "zmq.hpp"

namespace zcm {

  /**
   * @brief Base Operation class
   */
  class Base_Operation {
  public:

    /**
     * @brief Construct a base operation
     * @param[in] name Name of the operation
     * @param[in] priority Priority of the operation
     */
    Base_Operation(std::string name,
		   unsigned int priority) :
      name(name),
      priority(priority) {}

    /**
     * @brief Return the operation name
     * @return Name of the operation
     */  
    std::string get_name();

    /**
     * @brief Return the operation priority
     * @return Priority of the operation
     */    
    unsigned int get_priority() const;

    /**
     * @brief Virtual execute function overridden by concrete types 
     */
    virtual void execute() {}

  private:

    /** @brief Name of the Operation */    
    std::string name;

    /** @brief Priority of the Operation */    
    unsigned int priority;
  };

  /**
   * @brief Timer Operation class
   */
  class Timer_Operation : public Base_Operation {
  public:

    /**
     * @brief Construct a timer operation
     * @param[in] name Name of the operation
     * @param[in] priority Priority of the operation
     * @param[in] operation_function Timer function
     */  
    Timer_Operation(std::string name,
		    unsigned int priority,
		    std::function<void()> operation_function) :
      Base_Operation(name, priority),
      operation_function(operation_function) {}

    /**
     * @brief Timer operation function 
     */  
    void execute();

  private:

    /** @brief Timer operation function */    
    std::function<void()> operation_function;
  };

  /**
   * @brief Subscriber Operation class
   */
  class Subscriber_Operation : public Base_Operation {
  public:

    /**
     * @brief Construct a subscriber operation
     * @param[in] name Name of the operation
     * @param[in] priority Priority of the operation
     * @param[in] operation_function Subscriber function
     */    
    Subscriber_Operation(std::string name,
			 unsigned int priority,
			 std::function<void()> operation_function) :
      Base_Operation(name, priority),
      operation_function(operation_function) {}

    /**
     * @brief Subscriber operation function 
     */    
    void execute();

  private:

    /** @brief Subscriber Operation Function */      
    std::function<void()> operation_function;
  };

  /**
   * @brief Server Operation class
   */
  class Server_Operation : public Base_Operation {
  public:

    /**
     * @brief Construct a server operation
     * @param[in] name Name of the operation
     * @param[in] priority Priority of the operation
     * @param[in] operation_function Server function
     * @param[in] socket_ptr Pointer to the Server ZMQ socket
     * @param[in] recv_ready Pointer to the Server ready variable
     */   
    Server_Operation(std::string name,
		     unsigned int priority,
		     std::function<void()> operation_function,
		     zmq::socket_t * socket_ptr,
		     bool * recv_ready,
		     std::string * response) :
      Base_Operation(name, priority),
      operation_function(operation_function),
      socket_ptr(socket_ptr),
      recv_ready(recv_ready),
      response_ptr(response) {}

    /**
     * @brief Server operation function 
     */  
    void execute();

    /**
     * @brief Get the ZMQ server socket pointer 
     */  
    zmq::socket_t * get_socket_ptr();

    /**
     * @brief Get the response string pointer
     */
    std::string * get_response_ptr();

    /**
     * @brief Get the ZMQ server "ready" variable 
     */  
    void set_ready();

  private:

    /** @brief Server Operation Function */        
    std::function<void()> operation_function;

    /** @brief Pointer to the Server ZMQ socket */        
    zmq::socket_t * socket_ptr;

    /** @brief Pointer to the Server "ready" variable */        
    bool * recv_ready;
    
    /** @brief Pointer to the response string */
    std::string * response_ptr;
  };

}

#endif
