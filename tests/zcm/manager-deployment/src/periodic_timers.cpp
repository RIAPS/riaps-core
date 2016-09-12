/** @file    periodic_timers.cpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file contains definitions for the Timers_Component class
 */


#include "periodic_timers.hpp"

namespace zcm {

  /**
   * @brief Function required to dynamically load periodic_timers.so
   */      
  extern "C" {
    Component* create_component() {
      return new Timers_Component();
    }
  }     

  /**
   * @brief Construct a timer component
   * Register all operations exposed by this component
   */     
  Timers_Component::Timers_Component() {
    register_functionality("timer_1_function",
			   std::bind(&Timers_Component::timer_1_function, this));
    register_functionality("timer_2_function",
			   std::bind(&Timers_Component::timer_2_function, this));
    register_functionality("timer_3_function",
			   std::bind(&Timers_Component::timer_3_function, this));
    register_functionality("timer_4_function",
			   std::bind(&Timers_Component::timer_4_function, this));
    register_functionality("timer_5_function",
			   std::bind(&Timers_Component::timer_5_function, this));    
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */   
  void Timers_Component::timer_1_function() {
    std::cout << "Timer 1 expiry handled!" << std::endl;
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */    
  void Timers_Component::timer_2_function() {
    std::cout << "Timer 2 expiry handled!" << std::endl;
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */     
  void Timers_Component::timer_3_function() {
    std::cout << "Timer 3 expiry handled!" << std::endl;
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */     
  void Timers_Component::timer_4_function() {
    std::cout << "Timer 4 expiry handled!" << std::endl;
  }

  /**
   * @brief A timer operation
   * This operation can be triggered by a periodic timer
   * Bind this operation to a periodic timer in the JSON configuration
   */     
  void Timers_Component::timer_5_function() {
    std::cout << "Timer 5 expiry handled!" << std::endl;
  }  

}
