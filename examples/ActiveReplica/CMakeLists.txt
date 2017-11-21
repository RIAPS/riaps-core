include_directories(include)

add_library(client SHARED src/Client.cc
        src/base/ClientBase.cc
                                   include/messages/activereplica.capnp.c++
                                   include/messages/activereplica.capnp.h
                                   include/base/ClientBase.h
        include/Client.h
        include/base/GroupTypes.h
        )

add_library(server SHARED src/Server.cc
        src/base/ServerBase.cc
                                  include/messages/activereplica.capnp.c++
                                  include/messages/activereplica.capnp.h
        include/base/ServerBase.h
        include/Server.h
        include/base/GroupTypes.h
        )

add_library(sensor SHARED src/Sensor.cc
                          src/base/SensorBase.cc
                          include/messages/activereplica.capnp.c++
                          include/messages/activereplica.capnp.h
                          include/base/SensorBase.h
                          include/Sensor.h
        include/base/GroupTypes.h
        )

target_link_libraries(server czmq riaps dl capnp kj )
target_link_libraries(client czmq riaps dl capnp kj)
target_link_libraries(sensor czmq riaps dl capnp kj)

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