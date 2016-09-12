/** @file    server.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Server class
 */

#ifndef SERVER
#define SERVER
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <zmq.hpp>
#include "operation_queue.hpp"

namespace zcm {

  /**
   * @brief Server class
   */
  class Server {
  public:

    /**
     * @brief Construct a server object
     * @param[in] name Server name
     * @param[in] priority Priority of the server
     * @param[in] ZMQ Context of the Actor Process
     * @param[in] operation_function Operation function of the server
     * @param[in] operation_queue_ptr Pointer to the operation queue
     */    
    Server(std::string name,
	   unsigned int priority,
	   zmq::context_t * actor_context,
	   std::function<void()> operation_function,
	   Operation_Queue * operation_queue_ptr,
	   int timeout = 500) :
      name(name),
      priority(priority),
      context(actor_context),
      operation_function(operation_function),
      operation_queue_ptr(operation_queue_ptr),
      server_socket_timeout(timeout) {}

    /**
     * @brief Construct a server object with known endpoints
     * @param[in] name Server name
     * @param[in] priority Priority of the server
     * @param[in] ZMQ Context of the Actor Process
     * @param[in] endpoints A vector of endpoints to bind to
     * @param[in] operation_function Operation function of the server
     * @param[in] operation_queue_ptr Pointer to the operation queue
     */      
    Server(std::string name,
	   unsigned int priority,
	   zmq::context_t * actor_context,
	   std::vector<std::string> endpoints,
	   std::function<void()> operation_function,
	   Operation_Queue * operation_queue_ptr,
	   int timeout);

    /**
     * @brief Close the server socket and destroy the ZMQ context
     */  
    ~Server();

    /**
     * @brief Bind to a new set of endpoints
     * param[in] new_endpoints A new vector of endpoints to bind to
     */  
    void bind(std::vector<std::string> new_endpoints);

    /**
     * @brief Get the name of the server
     */  
    std::string get_name();

    /**
     * @brief Get the priority of the server
     */   
    unsigned int get_priority();

    /**
     * @brief Add a new connection to the server
     * @param[in] new_connection New connection address to bind to
     */      
    void add_connection(std::string new_connection);

    /**
     * @brief Thread function of the server
     * Behavior:
     * (1) Wait for a new request on the server ZMQ socket
     * (2) Create a Server Operation
     * (3) Enqueue onto operation_queue
     * (4) Goto step (1)
     */  
    void recv();
 
    /**
     * @brief Rebind the server operation function
     * @param[in] new_operation_function New server function to be handled upon recv() 
     */  
    void rebind_operation_function(std::function<void()> new_operation_function);

    /**
     * @brief Spawn a new thread for the server
     * @return Server thread
     */    
    std::thread spawn();

    /**
     * @brief Start the server thread
     */  
    void start();

    /**
     * @brief Is the message buffer empty?
     */
    bool is_buffer_empty();

    /**
     * @brief Is the message buffer empty?
     */
    std::string message();

    /**
     * @brief Set the response string 
     */
    void set_response(std::string new_response);

  private:

    /** @brief Name of the server */
    std::string name;

    /** @brief Priority of the server */
    unsigned int priority;

    /** @brief Pointer to the server ZMQ context */
    zmq::context_t * context;

    /** @brief Vector of connection endpoints */
    std::vector<std::string> endpoints;

    /** @brief Operation function bound to the server - Component method that handles received requests */
    std::function<void()> operation_function;

    /** @brief Pointer to the operation_queue */
    Operation_Queue * operation_queue_ptr;

    /** @brief Pointer to the server ZMQ socket */
    zmq::socket_t * server_socket;

    /** @brief Boolean representing the state of the server to receive new requests */
    bool ready;

    /** @brief Mutex used when changing operation_function at runtime */
    std::mutex func_mutex;

    /** @brief Buffer of received messages */
    std::queue<std::string> buffer;

    /** @brief response string to send to client */
    std::string * response;

    /** @brief Timeout of the server socket when replying to clients */
    int server_socket_timeout;

  };

}

#endif
