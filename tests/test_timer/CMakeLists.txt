include_directories(include)

add_library(generator SHARED src/Generator.cc
                             src/base/GeneratorBase.cc
                             include/messages/timertest.capnp.c++)

target_link_libraries(generator czmq riaps dl capnp kj soc)

add_library(receiver SHARED src/Receiver.cc
                            src/base/ReceiverBase.cc
                            include/messages/timertest.capnp.c++)

target_link_libraries(receiver czmq riaps dl capnp kj soc)

# add_executable(actor_prod src/prod/producer.cc
#                          src/messages.cc)

#target_link_libraries(comp_prod czmq riaps)
#target_link_libraries(actor_prod czmq riaps dl)

#add_library(comp_sub SHARED src/cons/comp_sub.cc)
#add_library(comp_rep SHARED src/cons/comp_rep.cc)
#add_executable(actor_cons src/cons/consumer.cc
 #src/messages.cc)

#target_link_libraries(comp_sub czmq riaps)
#target_link_libraries(comp_rep czmq riaps)


#target_link_libraries(actor_cons czmq riaps)