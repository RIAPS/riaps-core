/** @file    component.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Component class
 */

#ifndef COMPONENT
#define COMPONENT
#include "timer.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"
#include "client.hpp"
#include "server.hpp"
#include "zcm_endpoint.h"

namespace zcm {

  /**
   * @brief Component class
   */
  class Component {
  public:

    /**
     * @brief Construct a component
     * Prepare the component operation queue
     */
    Component();

    /**
     * @brief Destroy the component
     */  
    ~Component();

    /**
     * @brief Get pointer to operation_queue
     * @return Operation Queue pointer
     */
    Operation_Queue * get_operation_queue();

    /**
     * @brief Get a component timer by name
     * @param[in] timer_name Name of the timer
     */  
    Timer * timer(std::string timer_name);

    /**
     * @brief Get a component publisher by name
     * @param[in] publisher_name Name of the publisher
     */      
    Publisher * publisher(std::string publisher_name);

    /**
     * @brief Get a component subscriber by name
     * @param[in] subscriber_name Name of the subscriber
     */        
    Subscriber * subscriber(std::string subscriber_name);

    /**
     * @brief Get a component client by name
     * @param[in] client_name Name of the client
     */            
    Client * client(std::string client_name);

    /**
     * @brief Get a component server by name
     * @param[in] server_name Name of the server
     */      
    Server * server(std::string server_name);
    
    /**
     * @brief Add a timer to this component
     * @param[in] new_timer Pointer to a timer object
     */
    void add_timer(Timer * new_timer);

    /**
     * @brief Add a publisher to this component
     * @param[in] new_publisher Pointer to a publisher object
     */
    void add_publisher(Publisher * new_publisher);

    /**
     * @brief Add a subscriber to this component
     * @param[in] new_subscriber Pointer to a subscriber object
     */
    void add_subscriber(Subscriber * new_subscriber);

    /**
     * @brief Add a client to this component
     * @param[in] new_client Pointer to a client object
     */
    void add_client(Client * new_client);

    /**
     * @brief Add a server to this component
     * @param[in] new_server Pointer to a server object
     */
    void add_server(Server * new_server);

    /**
     * @brief Configure all component publishers
     * @param[in] publisher_endpoints A map of endpoints for all publishers
     */    
    void configure_publishers(std::map<std::string, 
			      std::vector<zcm_endpoint>> publisher_endpoints);

    /**
     * @brief Configure all component subscribers
     * @param[in] subscriber_endpoints A map of endpoints for all subscribers
     */        
    void configure_subscribers(std::map<std::string, 
			       std::vector<std::string>> subscriber_endpoints);

    /**
     * @brief Configure all component clients
     * @param[in] client_endpoints A map of endpoints for all clients
     */        
    void configure_clients(std::map<std::string, 
			   std::vector<std::string>> client_endpoints);

    /**
     * @brief Configure all component servers
     * @param[in] server_endpoints A map of endpoints for all servers
     */        
    void configure_servers(std::map<std::string, 
			   std::vector<std::string>> server_endpoints);

    /**
     * @brief Register component functionality
     * @param[in] operation_name Name of the operation
     * @param[in] operation_function The actual operation function
     */        
    void register_functionality(std::string operation_name,
				std::function<void()> operation_function);   

    /**
     * @brief Spawn the component executor thread
     * @return Return a pointer to the executor thread
     */    
    std::thread * spawn();

    /** @brief A map of all component operations */    
    std::map<std::string, std::function<void()>> functionality;      

  protected:

    /** @brief Pointer to the Component Operation Queue */  
    Operation_Queue * operation_queue;

    /** @brief Pointer to the Component Executor Thread */    
    std::thread * executor_thread;

    /** @brief A vector of component timers */
    std::vector<Timer*> timers;    

    /** @brief A vector of component publishers */
    std::vector<Publisher*> publishers;

    /** @brief A vector of component subscribers */
    std::vector<Subscriber*> subscribers;

    /** @brief A vector of component clients */
    std::vector<Client*> clients;

    /** @brief A vector of component servers */
    std::vector<Server*> servers;      
  };

}

#endif
