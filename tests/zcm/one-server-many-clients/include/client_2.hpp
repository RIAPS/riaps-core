/** @file    client_2.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Client_2 class
 */

#ifndef CLIENT_2
#define CLIENT_2
#include "component.hpp"

namespace zcm {

  /**
   * @brief Client_2 class
   */    
  class Client_2 : public Component {
  public:

    /**
     * @brief Construct Client_2 & register all exposed functionality
     */     
    Client_2();

    /**
     * @brief Method used to call the server 
     */         
    void call_the_server();

  };

}

#endif
