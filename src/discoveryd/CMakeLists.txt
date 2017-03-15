set(RDISCOVERYD_SOURCE r_discoveryd.cc
                       r_riaps_actor.cc
                       r_riaps_cmd_handler.cc
                       r_odht.cc
        )

add_executable(rdiscoveryd ${RDISCOVERYD_SOURCE})
target_link_libraries(rdiscoveryd czmq zmq opendht gnutls riaps capnp nettle hogweed gmp)

install(TARGETS rdiscoveryd DESTINATION bin)
