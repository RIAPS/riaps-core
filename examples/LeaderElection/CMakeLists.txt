include_directories(include)

add_library(leader SHARED src/Leader.cc
                     src/base/LeaderBase.cc
        include/Leader.h
        include/base/LeaderBase.h
        )


target_link_libraries(leader czmq riaps dl capnp kj )
