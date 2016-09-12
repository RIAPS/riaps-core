/** @file    component_2.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Component_2 class
 */

#ifndef COMPONENT_2
#define COMPONENT_2
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "component.hpp"

namespace zcm {

  /**
   * @brief Component_2 class
   */    
  class Component_2 : public Component {
  public:

    /**
     * @brief Construct component_2 & register all exposed functionality
     */     
    Component_2();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */         
    void timer_function();

    /**
     * @brief A server operation that can be requested by some client
     */         
    void server_function();

  };

}

#endif
