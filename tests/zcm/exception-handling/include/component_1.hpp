/** @file    component_1.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Component_1 class
 */

#ifndef COMPONENT_1
#define COMPONENT_1
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "component.hpp"

namespace zcm {

  /**
   * @brief Component_1 class
   */    
  class Component_1 : public Component {
  public:

    /**
     * @brief Construct component_1 & register all exposed functionality
     */     
    Component_1();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_1_function();

    /**
     * @brief A subscriber operation that can be bound to some subscriber
     */         
    void subscriber_function();

  };

}

#endif
