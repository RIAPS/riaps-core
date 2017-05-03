set(FRAMEWORK_LIB_FILES r_discoverdapi.cc
                        r_componentbase.cc
                        ../utils/r_utils.cc
                        ../utils/r_message.cc
                        ../utils/r_capnp_helper.cc
                        #../loggerd/r_logger_utilities.cc
                        r_subscriberport.cc
                        r_publisherport.cc
                        r_actor.cc
                        r_portbase.cc
                        r_periodictimer.cc
                        r_debugcomponent.cc
                        r_messagebase.cc
        #delete_r_network_interfaces.cc
                        r_responseport.cc
                        r_requestport.cc
                        r_parameter.cc
                        ../framework/rfw_network_interfaces.cc
                        ../framework/rfw_configuration.cc
                        ../messaging/r_message.cc
                        r_oneshottimer.cc
                        r_argumentparser.cc
        )

add_library(riaps SHARED ${FRAMEWORK_LIB_FILES})
target_link_libraries(riaps czmq zmq dl capnp kj)

add_executable(start_actor r_actor_main.cc)
target_link_libraries(start_actor czmq zmq riaps capnp)


install(TARGETS start_actor DESTINATION bin)
install(TARGETS riaps DESTINATION lib)
