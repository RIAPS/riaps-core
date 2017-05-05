set(RDEVMD_SOURCE r_devmd.cc
                  r_devm_actor.cc
                  r_devm_handlers.cc
                  ../../include/messaging/devm.capnp.c++
        )

add_executable(rdevmd ${RDEVMD_SOURCE})
target_link_libraries(rdevmd czmq zmq opendht gnutls riaps capnp nettle hogweed gmp)

install(TARGETS rdevmd DESTINATION bin)
