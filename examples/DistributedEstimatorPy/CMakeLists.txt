include_directories(include)

#include_directories(/usr/local/include/python3.5)
#include_directories(/usr/include/python3.5m)


add_library(globalestimatorpy SHARED
                                   include/base/GlobalEstimatorBase.h
                                   include/GlobalEstimator.h
                                   src/GlobalEstimator.cc
                                   src/base/GlobalEstimatorBase.cc
                                   include/messages/distributedestimator.capnp.c++)

add_library(localestimatorpy SHARED src/LocalEstimator.cc
                                  src/base/LocalEstimatorBase.cc
                                  include/messages/distributedestimator.capnp.c++
        )

add_library(sensorpy SHARED src/Sensor.cc
                          src/base/SensorBase.cc
                          include/messages/distributedestimator.capnp.c++
        )

target_link_libraries(localestimatorpy czmq riaps dl capnp kj )
target_link_libraries(globalestimatorpy czmq riaps dl capnp kj)
target_link_libraries(sensorpy czmq riaps dl capnp kj)

# add_executable(actor_prod src/prod/producer.cc
#                          src/messages.cc)

#target_link_libraries(comp_prod czmq riaps)
#target_link_libraries(actor_prod czmq riaps dl)

#add_library(comp_sub SHARED src/cons/comp_sub.cc)
#add_library(comp_rep SHARED src/cons/comp_rep.cc)
#add_executable(actor_cons src/cons/consumer.cc
 #src/messages.cc)

#target_link_libraries(comp_sub czmq riaps)
#target_link_libraries(comp_rep czmq riaps)


#target_link_libraries(actor_cons czmq riaps)