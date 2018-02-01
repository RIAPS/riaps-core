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

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@17c5bf5c (name: ATimer) (language: python, realtime: false)
add_library(ATimer 
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ATimer.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/ATimerBase.cc
			)
			
target_link_libraries(ATimer czmq riaps dl capnp kj)
install(TARGETS ATimer DESTINATION lib)

