#cmake_minimum_required(VERSION 3.10)
#project(DistributedVoltage)
#
#option(arch "amd64/armhf" "amd64")
#set(CMAKE_SYSTEM_NAME Linux)
#
#set(riaps_prefix "/opt/riaps/" CACHE STRING "the riaps prefix")
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-Wno-psabi")
#
#set(PYBIND11_CPP_STANDARD -std=c++17)
#
#
##Set the platform
#if (${arch} STREQUAL "armhf")
# set(TOOLCHAIN_PREFIX /usr/bin/arm-linux-gnueabihf)
# set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-7)
# set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
# set (CMAKE_C_FLAGS "-std=c99")
#else()
# set(CMAKE_C_COMPILER gcc-7)
# set(CMAKE_CXX_COMPILER g++-7)
# set (CMAKE_C_FLAGS "-std=c99")
#endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#set(CMAKE_INSTALL_PREFIX ${riaps_prefix}/${arch})
#
#set(DEPENDENCIES ${riaps_prefix})
#set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
#set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
#include_directories(${LIBALLPATH_INCLUDE})
#include_directories(${LIBALLPATH_INCLUDE}/pybind11/include)
#link_directories(${LIBALLPATH_LIB})
#
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(include)
include_directories(/usr/include/python3.6m/)

# riaps:keep_cmake:begin

# riaps:keep_cmake:end


add_custom_command(
        OUTPUT  "${CMAKE_SOURCE_DIR}/include/messages/distributedvoltage.capnp.c++"
        DEPENDS "${CMAKE_SOURCE_DIR}/distributedvoltage.capnp"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND capnp compile ./distributedvoltage.capnp -oc++:./include/messages/
        COMMENT "=== Generating capnp ==="
)

# riaps:keep_modbusuart:begin
add_library(modbusuart SHARED
        src/ModbusUART.cc
        src/base/ModbusUARTBase.cc
        include/base/ModbusUARTBase.h
        include/ModbusUART.h
        include/messages/distributedvoltage.capnp.c++
        )
target_link_libraries(modbusuart PRIVATE czmq riaps dl capnp kj modbus)
set_target_properties(modbusuart PROPERTIES PREFIX lib SUFFIX .so)
# riaps:keep_modbusuart:end




