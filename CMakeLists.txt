cmake_minimum_required(VERSION 3.0.0)
project(riaps_fw VERSION 0.0.0)

set (CMAKE_CXX_FLAGS "-std=c++11")

set(DEPENDENCIES /opt/riaps/)

#add includes
include_directories(include)

#Set the platform
include(CMakeLists_arm.txt)
#include(CMakeLists_amd64.txt)


add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)


# Tests
add_subdirectory(tests/test_pubsub)
add_subdirectory(tests/test_getservice_async)
