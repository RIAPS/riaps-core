cmake_minimum_required(VERSION 3.0.0)
project(riaps_core VERSION 0.1.0)

option(arch "amd64/armhf" "amd64")
option(BUILD_TESTS  "Build tests" ON)

set (CMAKE_CXX_FLAGS "-std=c++11")

set(DEPENDENCIES /opt/riaps/)

# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

#add includes
include_directories(include)

#Set the platform
if (${arch} STREQUAL "amd64")
    include(CMakeLists_amd64.txt)
elseif(${arch} STREQUAL "armhf")
    include(CMakeLists_arm.txt)
endif()

add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)


if (BUILD_TESTS)
    # Tests
    #add_subdirectory(tests/test_pubsub)
    #add_subdirectory(tests/test_getservice_async)
    #add_subdirectory(tests/test_pubsub_reqrep)
    add_subdirectory(tests/test_distributed_estimator)
endif()
