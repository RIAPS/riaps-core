/** @file    client_1.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Client_1 class
 */

#ifndef CLIENT_1
#define CLIENT_1
#include "component.hpp"

namespace zcm {

  /**
   * @brief Client_1 class
   */    
  class Client_1 : public Component {
  public:

    /**
     * @brief Construct Client_1 & register all exposed functionality
     */     
    Client_1();

    /**
     * @brief Method used to call the server 
     */         
    void call_the_server();

  };

}

#endif
