include_directories(include)

add_library(echo SHARED src/Echo.cc
                        src/base/EchoBase.cc
                        include/messages/EchoIO.capnp.c++)


add_library(iodevice SHARED src/IODevice.cc
                            src/base/IODeviceBase.cc
                            include/messages/EchoIO.capnp.c++)


target_link_libraries(echo czmq riaps dl capnp kj )
target_link_libraries(iodevice czmq riaps dl capnp kj )
