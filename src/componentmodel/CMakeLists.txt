set(FRAMEWORK_LIB_FILES r_discoverdapi.cc
                        r_componentbase.cc
                        ../utils/r_utils.cc
                        ../utils/r_message.cc
                        ../utils/r_capnp_helper.cc
                        #../loggerd/r_logger_utilities.cc
        r_insideport.cc
                        r_subscriberport.cc
                        r_publisherport.cc
                        r_actor.cc
                        r_portbase.cc
                        r_periodictimer.cc
                        r_debugcomponent.cc
                        r_messagebase.cc
                        r_responseport.cc
                        r_requestport.cc
                        r_parameter.cc
                        ../framework/rfw_network_interfaces.cc
                        ../framework/rfw_configuration.cc
                        ../messaging/r_message.cc
                        ../../include/messaging/disco.capnp.c++
                        #../../include/messaging/insideport.capnp.c++
                        r_oneshottimer.cc
                        r_commandlineparser.cc
                        r_argumentparser.cc
                        r_peripheral.cc
                        r_devmapi.cc
                        r_devicethread.cc
                        ../../include/componentmodel/r_discoverdapi.h
                        ../../include/componentmodel/r_componentbase.h
                        ../../include/componentmodel/r_publisherport.h
                        ../../include/componentmodel/r_subscriberport.h
                        ../../include/componentmodel/r_discoverdapi.h
                        ../../include/componentmodel/r_configuration.h
                        ../../include/componentmodel/r_periodictimer.h
                        ../../include/componentmodel/r_responseport.h
                        ../../include/componentmodel/r_requestport.h
                        ../../include/componentmodel/r_actor.h
                        ../../include/componentmodel/r_messagebase.h
                        ../../include/componentmodel/r_insideport.h
                        ../../include/componentmodel/r_oneshottimer.h
                        ../../include/componentmodel/r_commandlineparser.h
                        ../../include/const/r_const.h
        )

set(ACTOR_FILES r_actor_main.cc
                ../../include/componentmodel/r_actor.h
                ../../include/const/r_jsonmodel.h
                ../../include/const/r_const.h
                ../../include/boost/filesystem
                ../../include/componentmodel/r_argumentparser.h)

add_library(riaps SHARED ${FRAMEWORK_LIB_FILES})
target_link_libraries(riaps czmq zmq dl capnp kj)

add_executable(start_actor r_actor_main.cc)
target_link_libraries(start_actor czmq zmq riaps capnp)


install(TARGETS start_actor DESTINATION bin)
install(TARGETS riaps DESTINATION lib)
