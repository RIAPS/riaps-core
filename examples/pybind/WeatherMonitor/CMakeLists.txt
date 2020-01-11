include_directories(include)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#add_custom_command(
#        OUTPUT  "include/messages/TempData.capnp.c++"
#        DEPENDS "include/messages/TempData.capnp"
#        WORKING_DIRECTORY "include/messages"
#        COMMAND /opt/riaps/amd64/bin/capnp compile TempData.capnp -oc++:./
#        COMMENT "=== Generating capnp ==="
#)

add_library(tempsensor src/TempSensor.cc
        src/base/TempSensorBase.cc
        include/messages/TempData.capnp.c++
        include/base/TempSensorBase.h
        include/TempSensor.h
        )
target_link_libraries(tempsensor PRIVATE czmq riaps dl capnp kj)
set_target_properties(tempsensor PROPERTIES PREFIX lib SUFFIX .so)

add_library(tempmonitor src/TempMonitor.cc
        src/base/TempMonitorBase.cc
        include/messages/TempData.capnp.c++
        include/base/TempMonitorBase.h
        include/TempMonitor.h
        )
target_link_libraries(tempmonitor PRIVATE czmq riaps dl capnp kj)
set_target_properties(tempmonitor PROPERTIES PREFIX lib SUFFIX .so)
