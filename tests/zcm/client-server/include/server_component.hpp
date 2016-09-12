/** @file    server_component.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Server_Component class
 */

#ifndef SERVER_COMPONENT
#define SERVER_COMPONENT
#include "component.hpp"
#include "test_message.pb.h"

namespace zcm {

  /**
   * @brief Server_Component class
   */    
  class Server_Component : public Component {
  public:

    /**
     * @brief Construct a server component & register all exposed functionality
     */      
    Server_Component();

    /**
     * @brief A server operation that can be requested by some client
     */      
    void server_function();

  };

}

#endif
