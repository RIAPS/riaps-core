add_library(zcm SHARED actor.cpp  client.cpp  component.cpp  json.cpp  manager.cpp  operation_queue.cpp  operation_types.cpp  publisher.cpp  server.cpp  subscriber.cpp  timer.cpp servicebase.cc)
add_library(zcm_static STATIC actor.cpp  client.cpp  component.cpp  json.cpp  manager.cpp  operation_queue.cpp  operation_types.cpp  publisher.cpp  server.cpp  subscriber.cpp  timer.cpp servicebase.cc)


#install(TARGETS zcm LIBRARY DESTINATION lib)
#install(TARGETS zcm_static ARCHIVE DESTINATION lib)

