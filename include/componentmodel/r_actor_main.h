//
// Created by istvan on 11/7/16.
//

#ifndef RIAPS_FW_R_ACTOR_MAIN_H
#define RIAPS_FW_R_ACTOR_MAIN_H

#include <componentmodel/r_actor.h>
#include <framework/rfw_network_interfaces.h>

//nlohmann json
#include "json.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>



char* getCmdOption(char ** begin, char ** end, const std::string & option);

#endif //RIAPS_FW_R_ACTOR_MAIN_H
