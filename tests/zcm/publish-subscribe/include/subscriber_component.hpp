/** @file    subscriber_component.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Subscriber_Component class
 */

#ifndef SUBSCRIBER_COMPONENT
#define SUBSCRIBER_COMPONENT
#include "component.hpp"

namespace zcm {

  /**
   * @brief Subscriber_Component class
   */    
  class Subscriber_Component : public Component {
  public:

    /**
     * @brief Construct a subscriber component & register all exposed functionality
     */        
    Subscriber_Component();

    /**
     * @brief A subscriber operation that can be bound to a subscriber port
     */      
    void subscriber_function();

  };

}

#endif
