/** @file    operation_queue.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Operation_Queue class
 */

#ifndef OPERATION_QUEUE
#define OPERATION_QUEUE
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include "operation_types.hpp"

namespace zcm {

  /**
   * @brief Operation_Queue class
   */
  class Operation_Queue {
  public:

    /*
     * @brief Enqueue an operation onto the queue
     * @param[in] new_operation New operation to be enqueued
     */
    void enqueue(Base_Operation * new_operation);

    /*
     * @brief Dequeue the top operation to process the request
     */
    void dequeue();  

    /*
     * @brief Check if the queue is empty
     * @return true if queue is empty; false is queue has operations
     */
    bool empty();

    /*
     * @brief Return the top element i.e. first element in the queue
     * @return Pointer to the top element
     */
    Base_Operation * top();

    /*
     * @brief Queue processing function that binds to the component executor thread
     */  
    void process();

    /*
     * @brief Spawn the component executor thread
     */  
    std::thread * spawn();

    /*
     * @brief This struct enforces PFIFO scheduling scheme
     * The comparator function is bound to the operation_queue for enqueue ordering
     */    
    struct PriorityOrdering {
      bool operator()(const Base_Operation * lhs, const Base_Operation * rhs) const {
	return lhs->get_priority() < rhs->get_priority();
      }
    };

  private:

    /** @brief The component operation queue - STL priority_queue with fixed-priority scheduling  */
    std::priority_queue<Base_Operation, std::vector<Base_Operation*>, PriorityOrdering> operation_queue;

    /** @brief Mutex that protects the queue during enqueue/dequeue */
    std::mutex queue_mutex;
  };

}

#endif
