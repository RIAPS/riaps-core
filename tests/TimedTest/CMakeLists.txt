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

# Generating the headers and cpp for message edu.vanderbilt.riaps.app.impl.FStructTypeImpl@17811cbf (name: DummyT)
add_custom_command(OUTPUT  "${CMAKE_SOURCE_DIR}/messages-gen/DummyT.capnp.c++"
				   DEPENDS "${CMAKE_SOURCE_DIR}/messages-gen/DummyT.capnp" 
				   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}//messages-gen"  
				   COMMAND ${CMAKE_COMMAND} -E env "PATH=/opt/riaps/amd64/bin" capnp compile -oc++ "${CMAKE_SOURCE_DIR}/messages-gen/DummyT.capnp" --import-path="${CMAKE_SOURCE_DIR}//messages-gen"
				   )

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@3289c11c (name: Filter) (language: python, realtime: false)
add_library(filter 
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Filter.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/FilterBase.cc
			${CMAKE_SOURCE_DIR}/messages-gen/DummyT.capnp.c++
			)
			
target_link_libraries(filter czmq riaps dl capnp kj)
install(TARGETS filter DESTINATION lib)

# Creating Libraries for Component edu.vanderbilt.riaps.app.impl.ComponentImpl@723baf85 (name: Sensor) (language: python, realtime: false)
add_library(sensor 
			SHARED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Sensor.cc
			${CMAKE_CURRENT_SOURCE_DIR}/cpp/SensorBase.cc
			${CMAKE_SOURCE_DIR}/messages-gen/DummyT.capnp.c++
			)
			
target_link_libraries(sensor czmq riaps dl capnp kj)
install(TARGETS sensor DESTINATION lib)

