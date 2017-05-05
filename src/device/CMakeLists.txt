set(RDEVICE_SOURCE r_device.cc
        )

add_executable(start_device ${RDEVICE_SOURCE})
target_link_libraries(start_device czmq zmq opendht gnutls riaps capnp nettle hogweed gmp)

install(TARGETS start_device DESTINATION bin)
