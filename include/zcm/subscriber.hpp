/** @file    subscriber.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Subscriber class
 */

#ifndef SUBSCRIBER
#define SUBSCRIBER

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <zmq.hpp>
// Discovery service functions
#include "../../include/r_framework.h"
#include "operation_queue.hpp"

namespace zcm {

    /**
     * @brief Subscriber class
     */
    class Subscriber {
    public:

        /**
         * @brief Construct a subscriber object
         * @param[in] name Subscriber name
         * @param[in] priority Priority of the subscriber
         * @param[in] ZMQ Context of the Actor Process
         * @param[in] filter ZMQ filter for the subscriber
         * @param[in] operation_function Operation function of the subscriber
         * @param[in] operation_queue_ptr Pointer to the operation queue
         */
        Subscriber(std::string name,
                   unsigned int priority,
                   zmq::context_t *actor_context,
                   std::string filter,
                   std::function<void()> operation_function,
                   Operation_Queue *operation_queue_ptr) :
                name(name),
                priority(priority),
                context(actor_context),
                filter(filter),
                operation_function(operation_function),
                operation_queue_ptr(operation_queue_ptr) {}

        /**
         * @brief Construct a subscriber object with known endpoints
         * @param[in] name Subscriber name
         * @param[in] priority Priority of the subscriber
         * @param[in] ZMQ Context of the Actor Process
         * @param[in] filter ZMQ filter for the subscriber
         * @param[in] endpoints A vector of endpoints to connect to
         * @param[in] operation_function Operation function of the subscriber
         * @param[in] operation_queue_ptr Pointer to the operation queue
         */
        Subscriber(std::string name,
                   unsigned int priority,
                   zmq::context_t *actor_context,
                   std::string filter,
                   std::vector<std::string> publishernames,
                   std::function<void()> operation_function,
                   Operation_Queue *operation_queue_ptr);

        /**
         * @brief Close the subscriber socket and destroy the ZMQ context
         */
        ~Subscriber();

        /**
         * @brief Connect to a new set of endpoints
         * param[in] new_endpoints A new vector of endpoints to connect to
         */
        void connect(std::vector<std::string> new_endpoints);

        /**
         * @brief Get the name of the subscriber
         */
        std::string get_name();



        /**
         * @brief Get the priority of the subscriber
         */
        unsigned int get_priority();

        /**
         * @brief Add a new connection to the subscriber
         * @param[in] new_connection New connection address to connect to
         */
        void add_connection(std::string new_connection);

        /**
         * @brief Thread function of the subscriber
         * Behavior:
         * (1) Wait for a new message on the subscriber ZMQ socket
         * (2) Create a Susbcriber Operation
         * (3) Enqueue onto operation_queue
         * (4) Goto step (1)
         */
        void recv();

        /**
         * @brief Rebind the subscriber operation function
         * @param[in] new_operation_function New subscriber function to be handled upon recv()
         */
        void rebind_operation_function(std::function<void()> new_operation_function);

        /**
         * @brief Spawn a new thread for the subscriber
         * @return Subscriber thread
         */
        std::thread spawn();

        /**
         * @brief Start the subscriber thread
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

    protected:

        /**
         * @brief Returns a ZMQ publisher endpoint from consul by servicename (the name,
         *        which used in the service registration).
         * @return ZMQ endpoint (e.g.: tcp://192.168.1.100:5555). Returns empty string if no service registered with this name.
         */
        std::string get_publisher_address(std::string publishername);

        /** @brief Name of the subscriber */
        std::string name;

        /** @brief Priority of the subscriber */
        unsigned int priority;

        /** @brief Pointer to the subscriber ZMQ context */
        zmq::context_t *context;

        /** @brief Reception filter enforced on all received messages */
        std::string filter;

        /** @brief Vector of connection endpoints */
        //std::vector<std::string> endpoints;


        // TODO: this is a new thing, test + and document it
        std::vector<std::string> publishernames;

        /** @brief Operation function bound to the subscriber */
        std::function<void()> operation_function;

        /** @brief Pointer to the operation queue */
        Operation_Queue *operation_queue_ptr;

        /** @brief Pointer to the subscriber ZMQ socket */
        zmq::socket_t *subscriber_socket;

        /** @brief Mutex used to change operation_function at runtime */
        std::mutex func_mutex;

        /** @brief Buffer of messages received by the subscriber */
        std::queue<std::string> buffer;
    };

}

#endif
