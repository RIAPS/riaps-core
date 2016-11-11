set(RLOGGERD_SOURCE r_loggerd.cc)

add_executable(rloggerd ${RLOGGERD_SOURCE})
target_link_libraries(rloggerd czmq riaps zmq)