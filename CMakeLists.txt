cmake_minimum_required(VERSION 3.0.0)
project(riaps_fw VERSION 0.0.0)

set (CMAKE_CXX_FLAGS "-std=c++11")

set(DEPENDENCIES /opt/riaps/)

# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

#add includes
include_directories(include)

#Set the platform
#include(CMakeLists_arm.txt)
include(CMakeLists_amd64.txt)

add_subdirectory(opendht)


add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)
add_subdirectory(src/loggerd)


# Tests
#add_subdirectory(tests/test_pubsub)
#add_subdirectory(tests/test_getservice_async)
#add_subdirectory(tests/test_pubsub_reqrep)

add_subdirectory(tests/test_distributed_estimator)
