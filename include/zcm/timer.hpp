/** @file    timer.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Timer class
 */

#ifndef TIMER
#define TIMER
#include <iostream>
#include <string>
#include <chrono>
#include <ratio>
#include <thread>
#include "operation_queue.hpp"

namespace zcm {

  /**
   * @brief Timer class
   */
  class Timer {
  public:

    /**
     * @brief Construct a timer
     * @param[in] name Name of the timer
     * @param[in] priority Priority of the timer
     * @param[in] period Period of the timer in nanoseconds
     * @param[in] operation_function Operation to which the timer is bound
     * @param[in] operation_queue_ptr Pointer to the operation_queue
     */
    Timer(std::string name, unsigned int priority, long long period, 
	  std::function<void()> operation_function, 
	  Operation_Queue * operation_queue_ptr);

    /**
     * @brief Timer thread function
     * Behavior:
     * (1) Wait for timer expiry
     * (2) Create a Timer_Operation
     * (3) Enqueue onto operation_queue
     * (4) Goto step (1)
     */
    void operation();

    /**
     * @brief Get the timer name
     * @return Timer name
     */
    std::string get_name();

    /**
     * @brief Get the timer priority
     * @return Timer priority
     */  
    unsigned int get_priority();

    /**
     * @brief Change the timer period
     * @param[in] new_period New timer period in nanoseconds
     */  
    void change_period(long long new_period);

    /**
     * @brief Rebind the timer operation function
     * @param[in] new_operation_function New timer function to be handled upon expiry 
     */    
    void rebind_operation_function(std::function<void()> new_operation_function);

    /**
     * @brief Spawn a new thread for the timer
     * @return Timer thread
     */    
    std::thread spawn();

    /**
     * @brief Start the timer thread
     */
    void start();
  

  private:

    /** @brief Name of the timer */
    std::string name;

    /** @brief Priority of the timer */
    unsigned int priority;

    /** @brief Period of the timer */
    std::chrono::duration<long long, std::ratio<1, 1000000000>> period;
  
    /** @brief Operation function bound to the timer */
    std::function<void()> operation_function;

    /** @brief Pointer to the operation queue */
    Operation_Queue * operation_queue_ptr;

    /** @brief Mutex used to change the timer period at runtime */
    std::mutex period_mutex;

    /** @brief Mutex used to change the operation_function at runtime */
    std::mutex func_mutex; 
  };

} 

#endif
