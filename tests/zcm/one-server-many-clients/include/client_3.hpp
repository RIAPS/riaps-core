/** @file    client_3.hpp 
 *  @author  Pranav Srinivas Kumar
 *  @date    2016.04.24
 *  @brief   This file declares the Client_3 class
 */

#ifndef CLIENT_3
#define CLIENT_3
#include "component.hpp"

namespace zcm {

  /**
   * @brief Client_3 class
   */    
  class Client_3 : public Component {
  public:

    /**
     * @brief Construct Client_3 & register all exposed functionality
     */     
    Client_3();

    /**
     * @brief Method used to call the server 
     */         
    void call_the_server();

  };

}

#endif
