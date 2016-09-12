/** @file    server_1.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Server_1 class
 */

#ifndef SERVER_1
#define SERVER_1
#include "component.hpp"

namespace zcm {

  /**
   * @brief Server_1 class
   */    
  class Server_1 : public Component {
  public:

    /**
     * @brief Construct Server_1 & register all exposed functionality
     */     
    Server_1();

    /**
     * @brief A server operation that can be requested by some client
     */         
    void server_function();

  };

}

#endif
