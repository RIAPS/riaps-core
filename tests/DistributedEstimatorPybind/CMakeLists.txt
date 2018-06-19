#cmake_minimum_required(VERSION 3.0)
#project(distributedestimatorpybind)

#set(CMAKE_SYSTEM_NAME Linux)
#set(RIAPS_PREFIX /opt/riaps/amd64/)
#set(ARCH amd64)
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_FIND_ROOT_PATH /usr/)
#set (CMAKE_CXX_FLAGS "-std=c++11")
#set (CMAKE_C_FLAGS "-std=c99")
#set(DEPENDENCIES ${RIAPS_PREFIX})
#set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/include)
#set (LIBALLPATH_LIB ${DEPENDENCIES}/lib)
#link_directories(${LIBALLPATH_LIB})
#include_directories(${LIBALLPATH_INCLUDE})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



include_directories(include)

add_library(converter STATIC include/converter.h src/converter.cc)

pybind11_add_module(globalestimator src/GlobalEstimator.cc
                                    src/base/GlobalEstimatorBase.cc
                                    include/messages/distributedestimator.capnp.c++
                                    include/base/GlobalEstimatorBase.h
                                    include/GlobalEstimator.h
                                    )
target_link_libraries(globalestimator PRIVATE czmq riaps dl capnp kj converter)

#pybind11_add_module(inh ./testinh/Child.cc ./testinh/Child.h)
#target_link_libraries(inh PRIVATE parent)

#add_library(globalestimator SHARED src/GlobalEstimator.cc
#                                   src/base/GlobalEstimatorBase.cc
#                                   include/messages/distributedestimator.capnp.c++)
#
#add_library(localestimator SHARED src/LocalEstimator.cc
#                                  src/base/LocalEstimatorBase.cc
#                                  include/messages/distributedestimator.capnp.c++
#        )
#
#add_library(sensor SHARED src/Sensor.cc
#                          src/base/SensorBase.cc
#                          include/messages/distributedestimator.capnp.c++
#        )
#
#target_link_libraries(localestimator czmq riaps dl capnp kj )
#target_link_libraries(globalestimator czmq riaps dl capnp kj)
#target_link_libraries(sensor czmq riaps dl capnp kj)