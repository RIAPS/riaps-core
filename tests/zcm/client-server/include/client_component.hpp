/** @file    client_component.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Client_Component class
 */

#ifndef CLIENT_COMPONENT
#define CLIENT_COMPONENT
#include "component.hpp"
#include "test_message.pb.h"

namespace zcm {

  /**
   * @brief Client_Component class
   */  
  class Client_Component : public Component {
  public:

    /**
     * @brief Construct a client component & register all exposed functionality
     */    
    Client_Component();

    /**
     * @brief A timer operation that can be triggered by some periodic timer
     */      
    void timer_function();

  };

}

#endif
