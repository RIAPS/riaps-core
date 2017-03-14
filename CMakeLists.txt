cmake_minimum_required(VERSION 3.0.0)
project(riaps_core VERSION 0.1.0)

option(arch "amd64/armhf" "amd64")
option(BUILD_TESTS  "Build tests" ON)
set(CMAKE_SYSTEM_NAME Linux)

#Set the platform
if (${arch} STREQUAL "armhf")
set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
else()
set(CMAKE_C_COMPILER gcc-5)
set(CMAKE_CXX_COMPILER g++-5)
endif()
set(DEPENDENCIES /opt/riaps/)
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
include_directories(${LIBALLPATH_INCLUDE})
link_directories(${LIBALLPATH_LIB})

set (CMAKE_CXX_FLAGS "-std=c++11")
set (CMAKE_C_FLAGS "-std=c99")

# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

#add includes
include_directories(include)
add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)
if (BUILD_TESTS)
include(CTest)
enable_testing()
    # Tests
    #add_subdirectory(tests/test_pubsub)
    #add_subdirectory(tests/test_getservice_async)
    #add_subdirectory(tests/test_pubsub_reqrep)
    add_subdirectory(tests/test_distributed_estimator)
endif()
