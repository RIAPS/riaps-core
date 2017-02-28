include_directories(include)


add_executable(actor_getservice_async ./src/actor_getservice_async.cc ./src/actor.cc ./src/component_pub.cc)
target_link_libraries(actor_getservice_async czmq riaps)
