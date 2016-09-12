/** @file    publisher_component.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Publisher_Component class
 */

#ifndef PUBLISHER_COMPONENT
#define PUBLISHER_COMPONENT
#include "component.hpp"

namespace zcm {

  /**
   * @brief Publisher_Component class
   */   
  class Publisher_Component : public Component {
  public:

    /**
     * @brief Construct a publisher component & register all exposed functionality
     */       
    Publisher_Component();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */     
    void timer_1_function();

  };

}

#endif
