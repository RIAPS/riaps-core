set(FRAMEWORK_LIB_FILES r_discoverdapi.cc
                        r_componentbase.cc
                        ../utils/r_utils.cc
                        ../utils/r_message.cc
                        r_subscriberport.cc
                        r_publisherport.cc
                        r_actor.cc
                        r_portbase.cc
                        r_timer.cc
                        r_responseport.cc
                        r_requestport.cc)

add_library(riaps SHARED ${FRAMEWORK_LIB_FILES})
target_link_libraries(riaps czmq)