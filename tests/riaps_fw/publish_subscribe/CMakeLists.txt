

set(SUB_FILES ./subscriber/src/subscriber_component.cc)
set(PUB_FILES ./publisher/src/publisher_component.cc)

set(ACTOR_SUB_FILES ./subscriber/src/main.cc)
set(ACTOR_PUB_FILES ./publisher/src/main.cc)


add_library(subscriber_component SHARED ${SUB_FILES})
add_library(publisher_component SHARED ${PUB_FILES})

add_executable(actor_sub ${ACTOR_SUB_FILES})
target_link_libraries(actor_sub zcm zmq dl rdiscovery)

add_executable(actor_pub ${ACTOR_PUB_FILES})
target_link_libraries(actor_pub zcm zmq dl rdiscovery)


file(COPY publisher/config/pub_conf.json DESTINATION ${CMAKE_SOURCE_DIR}/build/Debug)
file(COPY subscriber/config/sub_conf.json DESTINATION ${CMAKE_SOURCE_DIR}/build/Debug)