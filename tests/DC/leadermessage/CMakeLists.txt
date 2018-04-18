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


# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@53a26943 (name: CompOne) (language: python, realtime: false)
#add_library(compone
#			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompOne.cc
#			${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompOneBase.cc
#			)
#
#target_link_libraries(compone czmq riaps dl capnp kj)
#install(TARGETS compone DESTINATION lib)
#
## Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@215cd1bf (name: CompThree) (language: python, realtime: false)
#add_library(compthree
#			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompThree.cc
#			${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompThreeBase.cc
#			)
#
#target_link_libraries(compthree czmq riaps dl capnp kj)
#install(TARGETS compthree DESTINATION lib)
#
## Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@7da7baec (name: CompTwo) (language: python, realtime: false)
#add_library(comptwo
#			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompTwo.cc
#			${CMAKE_CURRENT_SOURCE_DIR}/cpp/CompTwoBase.cc
#			)
#
#target_link_libraries(comptwo czmq riaps dl capnp kj)
#install(TARGETS comptwo DESTINATION lib)


include_directories(./cpp/include)
include_directories(./messages-gen)

add_library(compone
        SHARED ./cpp/CompOne.cc
        ./cpp/CompOneBase.cc
        ./cpp/include/common.h
        ./cpp/include/CompOne.h
        ./cpp/include/CompOneBase.h
        )

target_link_libraries(compone czmq riaps dl capnp kj)
install(TARGETS compone DESTINATION lib)



