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
