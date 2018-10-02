include_directories(include)

add_library(consensus SHARED
        src/Consensus.cc
        src/base/ConsensusBase.cc
        include/messages/consensus.capnp.c++
        include/Consensus.h
        include/base/ConsensusBase.h
        include/messages/consensus.capnp.h
        )


target_link_libraries(consensus czmq riaps dl capnp kj )
