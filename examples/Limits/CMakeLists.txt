cmake_minimum_required(VERSION 3.0)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${ARCH}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#Library Dependencies
set(DEPENDENCIES ${RIAPS_PREFIX})
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/lib)
link_directories(${LIBALLPATH_LIB})
include_directories(${CMAKE_SOURCE_DIR}//messages-gen)
include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@561ff90c (name: CPULimit) (language: python, realtime: false)
add_library(CPULimit 
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CPULimit.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/CPULimitBase.cc
			)
			
target_link_libraries(CPULimit czmq riaps dl capnp kj)
install(TARGETS CPULimit DESTINATION lib)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@60e34fa1 (name: MemLimit) (language: python, realtime: false)
add_library(MemLimit 
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/MemLimit.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/MemLimitBase.cc
			)
			
target_link_libraries(MemLimit czmq riaps dl capnp kj)
install(TARGETS MemLimit DESTINATION lib)

