include_directories(include)

add_library(globalestimator SHARED src/GlobalEstimator.cc
                                   src/base/GlobalEstimatorBase.cc
                                   include/messages/distributedestimator.capnp.c++)

add_library(localestimator SHARED src/LocalEstimator.cc
                                  src/base/LocalEstimatorBase.cc
                                  include/messages/distributedestimator.capnp.c++
        )

add_library(sensor SHARED src/Sensor.cc
                          src/base/SensorBase.cc
                          include/messages/distributedestimator.capnp.c++
        )

target_link_libraries(localestimator czmq riaps dl capnp kj )
target_link_libraries(globalestimator czmq riaps dl capnp kj)
target_link_libraries(sensor czmq riaps dl capnp kj)