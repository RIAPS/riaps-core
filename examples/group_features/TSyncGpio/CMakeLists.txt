#cmake_minimum_required(VERSION 3.0)
#set(CMAKE_SYSTEM_NAME Linux)
#set(CMAKE_CXX_FLAGS "-std=c++11")
#set(CMAKE_C_FLAGS "-std=c99")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${ARCH}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
##Library Dependencies
#set(DEPENDENCIES ${RIAPS_PREFIX})
#set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/include)
#set (LIBALLPATH_LIB ${DEPENDENCIES}/lib)
#link_directories(${LIBALLPATH_LIB})
#include_directories(${CMAKE_SOURCE_DIR}//messages-gen)
#include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@1389af23 (name: Tsca) (language: python, realtime: false)
add_library(tscagpio
			SHARED cpp/TscaGpio.cc
        cpp/TscaGpioBase.cc
        cpp/include/TscaGpioBase.h
        cpp/include/TscaGpio.h
			)
			
target_link_libraries(tscagpio czmq riaps dl capnp kj soc)
install(TARGETS tscagpio DESTINATION lib)

