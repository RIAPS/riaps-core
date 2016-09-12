/** @file    component_3.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Component_3 class
 */

#ifndef COMPONENT_3
#define COMPONENT_3
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <exception>
#include "component.hpp"

namespace zcm {

  /**
   * @brief Component_3 class
   */    
  class Component_3 : public Component {
  public:

    /**
     * @brief Construct component_3 & register all exposed functionality
     */     
    Component_3();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */      
    void timer_function();

  };

}

#endif
