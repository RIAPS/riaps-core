/** @file    timer.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Timer class
 */

#include <unistd.h>
#include "../../include/zcm/timer.hpp"

namespace zcm {

  // Construct the periodic timer
  Timer::Timer(std::string name, unsigned int priority, long long period, 
	       std::function<void()> operation_function, 
	       Operation_Queue * operation_queue_ptr) : 
    name(name), 
    priority(priority),
    period(std::chrono::nanoseconds(period)), 
    operation_function(operation_function),
    operation_queue_ptr(operation_queue_ptr) {}

  // Timer thread function
  void Timer::operation() {
    while(true) {
      period_mutex.lock();
      auto start = std::chrono::high_resolution_clock::now();
      while(std::chrono::duration_cast<std::chrono::nanoseconds>
	    (std::chrono::high_resolution_clock::now() - start) < period) {}
      period_mutex.unlock();

      func_mutex.lock();
      Timer_Operation * new_operation = new Timer_Operation(name, priority, 
							    operation_function);
      operation_queue_ptr->enqueue(new_operation);
      func_mutex.unlock();

      //TODO: Remove usleep
      usleep(500);
    }  
  }

  // Return the timer name
  std::string Timer::get_name() {
    return name;
  }

  // Return the timer priority
  unsigned int Timer::get_priority() {
    return priority;
  }

  // Change the timer period
  void Timer::change_period(long long new_period) {
    period_mutex.lock();
    period = std::chrono::nanoseconds(new_period);
    std::cout << "Changed Timer Period to: " << new_period << std::endl;
    period_mutex.unlock();
  }

  // Rebind the timer operation function
  void Timer::rebind_operation_function(std::function<void()> new_operation_function) {
    func_mutex.lock();
    operation_function = new_operation_function;
    func_mutex.unlock();
  }

  // Spawn and return a timer thread
  std::thread Timer::spawn() {
    return std::thread(&Timer::operation, this);
  }

  // Start the timer
  void Timer::start() {
    std::thread timer_thread = spawn();
    timer_thread.detach();
  }

}
