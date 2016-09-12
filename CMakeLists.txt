cmake_minimum_required(VERSION 3.0.0)
project(riaps_fw VERSION 0.0.0)

set (CMAKE_CXX_FLAGS "-std=c++11")

#add includes
include_directories(include)

#Set the platform
include(CMakeLists_arm.txt)
#include(CMakeLists_amd64.txt)


add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)


# Tests
add_subdirectory(tests/actor_test)




#include_directories(include/zcm)
#include_directories(include/zmq)



#set(SOURCE_FILES discoveryd/r_discoveryd.cc
#                 ./src/r_utils.cc
#                 ./src/r_consul.cc
#                 ./src/r_http_api.cc
#                 ./src/r_consul_actor.cc
#        discoveryd/r_riaps_actor.cc)





#add_subdirectory(./src/zcm)
#add_subdirectory(./tests/riaps_fw/publish_subscribe)