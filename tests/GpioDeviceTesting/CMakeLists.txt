include_directories(include)

add_library(togglegpiocomponent SHARED src/ToggleGpioComponent.cc
                                       src/base/ToggleGpioComponentBase.cc
                                       include/messages/GpioToggleExample.capnp.c++)

add_library(gpiodevicecomponent SHARED src/GpioDeviceComponent.cc
                                       src/base/GpioDeviceComponentBase.cc
                                       src/GpioDeviceThread.cc
                                       include/messages/GpioToggleExample.capnp.c++)

target_link_libraries(togglegpiocomponent czmq riaps dl capnp kj )
target_link_libraries(gpiodevicecomponent czmq riaps dl capnp kj )