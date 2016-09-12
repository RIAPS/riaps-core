/*
 * Periodic Timers Component
 * Author: Pranav Srinivas Kumar
 * Date: 2016.04.23
 */

/** @file    periodic_timers.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Timers_Component class
 */

#ifndef TIMERS_COMPONENT
#define TIMERS_COMPONENT
#include "component.hpp"

namespace zcm {

  /**
   * @brief Timers_Component class
   */    
  class Timers_Component : public Component {
  public:

    /**
     * @brief Construct a timers component & register all exposed functionality
     */       
    Timers_Component();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_1_function();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_2_function();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_3_function();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_4_function();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_5_function();
  };

}

#endif
