set(RDISCOVERYD_SOURCE r_discoveryd.cc
                       r_riaps_actor.cc
                       r_riaps_cmd_handler.cc
                       r_odht.cc
                       ../../include/messaging/disco.capnp.c++
                       ../../include/messaging/disco.capnp.h
                       ../../include/discoveryd/r_riaps_actor.h
                       ../../include/discoveryd/r_riaps_cmd_handler.h
                       ../../include/discoveryd/r_discoveryd_commands.h
                       ../../include/discoveryd/r_odht.h
                       ../../include/framework/rfw_configuration.h
                       ../../include/framework/rfw_network_interfaces.h
                       ../../include/utils/r_utils.h
        )

add_executable(rdiscoveryd ${RDISCOVERYD_SOURCE})
target_link_libraries(rdiscoveryd czmq zmq opendht gnutls riaps capnp nettle hogweed gmp glog)

install(TARGETS rdiscoveryd DESTINATION bin)
