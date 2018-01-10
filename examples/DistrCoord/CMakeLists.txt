include_directories(include)

add_library(distrcoord SHARED
        src/DistrCoord.cc
        src/base/DistrCoordBase.cc
        include/messages/distrcoord.capnp.c++
        include/DistrCoord.h
        include/base/DistrCoordBase.h
        include/messages/distrcoord.capnp.h
        )


target_link_libraries(distrcoord czmq riaps dl capnp kj )
