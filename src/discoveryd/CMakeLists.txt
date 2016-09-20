set(RDISCOVERYD_SOURCE r_discoveryd.cc
                       r_consul_actor.cc
                       r_riaps_actor.cc
                       r_consul.cc
                       r_http_api.cc
                       r_riaps_cmd_handler.cc
                       r_service_poller.cc)

add_executable(rdiscoveryd ${RDISCOVERYD_SOURCE})
target_link_libraries(rdiscoveryd czmq neon riaps)