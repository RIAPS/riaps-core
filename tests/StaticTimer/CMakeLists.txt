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

# Generating the headers and cpp for message edu.vanderbilt.riaps.app.impl.FStructTypeImpl@5490b284 (name: Msg)
#add_custom_command(OUTPUT  "${CMAKE_SOURCE_DIR}/messages-gen/Msg.capnp.c++"
#		DEPENDS "${CMAKE_SOURCE_DIR}/messages-gen/Msg.capnp"
#		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}//messages-gen"
#		COMMAND ${CMAKE_COMMAND} -E env "PATH=/opt/riaps/amd64/bin" capnp compile -oc++ "${CMAKE_SOURCE_DIR}/messages-gen/Msg.capnp" --import-path="${CMAKE_SOURCE_DIR}//messages-gen"
#		)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@146aca6f (name: Producer) (language: python, realtime: false)
add_library(producer
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Producer.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/ProducerBase.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/messages-gen/Msg.capnp.c++
			)
			
target_link_libraries(producer czmq riaps dl capnp kj)
install(TARGETS producer DESTINATION lib)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@6a56a8e6 (name: STimer) (language: python, realtime: false)
add_library(stimer
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/STimer.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/STimerBase.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/messages-gen/Msg.capnp.c++
			)
			
target_link_libraries(stimer czmq riaps dl capnp kj)
install(TARGETS stimer DESTINATION lib)

