include_directories(include)

add_library(comp_prod SHARED src/prod/comp_gps.cc)
# add_executable(actor_prod src/prod/producer.cc
#                          src/messages.cc)

target_link_libraries(comp_prod czmq riaps)
#target_link_libraries(actor_prod czmq riaps dl)

add_library(comp_sub SHARED src/cons/comp_sub.cc)
add_library(comp_rep SHARED src/cons/comp_rep.cc)
#add_executable(actor_cons src/cons/consumer.cc
 #src/messages.cc)

target_link_libraries(comp_sub czmq riaps)
target_link_libraries(comp_rep czmq riaps)


#target_link_libraries(actor_cons czmq riaps)