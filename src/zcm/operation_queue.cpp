/** @file    operation_queue.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Operation_Queue class
 */

#include "../../include/zcm/operation_queue.hpp"

namespace zcm {

  // Enqueue a new operation onto the operation queue
  void Operation_Queue::enqueue(Base_Operation * new_operation) {
    queue_mutex.lock();
    operation_queue.push(new_operation);
    queue_mutex.unlock();
  }

  // Dequeue the first operation from the queue
  void Operation_Queue::dequeue() {
    if (!empty())
      operation_queue.pop();
  }

  // Check if the operation queue is empty
  bool Operation_Queue::empty() {
    return operation_queue.empty();
  }

  // Return the first operation from the queue
  Base_Operation * Operation_Queue::top() {
    return operation_queue.top();
  }

  // Thread function to process requests in the operation queue
  void Operation_Queue::process() {
    while(true) {
      if (operation_queue.size() > 0) {
	queue_mutex.lock();
	Base_Operation * top_operation = operation_queue.top();
	dequeue();
	queue_mutex.unlock();
	top_operation->execute();	  
      }
    }
  }

  // Spawn the executor thread that processes the queue
  std::thread * Operation_Queue::spawn() {
    return new std::thread(&Operation_Queue::process, this);
  }

}





