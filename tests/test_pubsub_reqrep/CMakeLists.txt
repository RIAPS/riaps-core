include_directories(include)

add_executable(actor_prod src/prod/producer.cc
                          src/prod/actor_prod.cc
                          src/prod/comp_gps.cc
                          src/messages.cc)
target_link_libraries(actor_prod czmq riaps)

add_executable(actor_cons src/cons/consumer.cc
                          src/cons/actor_cons.cc
                          src/cons/comp_sub.cc
                          src/messages.cc)
target_link_libraries(actor_cons czmq riaps)