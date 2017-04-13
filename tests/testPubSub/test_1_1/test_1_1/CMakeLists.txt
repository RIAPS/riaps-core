include_directories(include)

add_library(collector SHARED src/collector.cc)
add_library(temperaturesensor SHARED src/temperaturesensor.cc)

target_link_libraries(collector czmq riaps dl capnp kj )
target_link_libraries(temperaturesensor czmq riaps )

#cmake_minimum_required(VERSION 3.0.0)
#project(riaps_core VERSION 0.1.0)
#
#set (CMAKE_CXX_FLAGS "-std=c++11")
#
#set(DEPENDENCIES /opt/riaps/)
#
## Debug binaries are to be copied into "./bin" directory
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
#
##add includes
#include_directories(include)
#include_directories(../../../../include)
#
##Set the platform
##include(CMakeLists_arm.txt)
#include(CMakeLists_amd64.txt)
#
#
#set(TEST_COLLECTOR_LIB_FILES src/collector.cc)
#set(TEST_SENSOR_LIB_FILES src/temperaturesensor.cc)
#
#
#add_library(collector SHARED ${TEST_COLLECTOR_LIB_FILES})
#target_link_libraries(collector riaps czmq dl capnp kj)
#
