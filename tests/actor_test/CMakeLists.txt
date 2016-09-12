add_executable(actor_pub pub/actor_sub.cc)
target_link_libraries(actor_pub czmq riaps)

add_executable(actor_sub ./sub/actor_sub.cc)
target_link_libraries(actor_sub czmq riaps)