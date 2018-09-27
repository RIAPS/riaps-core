include_directories(include)

add_custom_command(
        OUTPUT  "include/messages/TempData.capnp.c++"
        DEPENDS "include/messages/TempData.capnp"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/messages"
        COMMAND /opt/riaps/amd64/bin/capnp compile TempData.capnp -oc++:./
        COMMENT "=== Generating capnp ==="
)

pybind11_add_module(tempsensor src/TempSensor.cc
                               src/base/TempSensorBase.cc
                               ${CMAKE_CURRENT_SOURCE_DIR}/include/messages/TempData.capnp.c++
                               include/base/TempSensorBase.h
                               include/TempSensor.h
        )
target_link_libraries(tempsensor PRIVATE czmq riaps dl capnp kj)
set_target_properties(tempsensor PROPERTIES PREFIX lib SUFFIX .so)

pybind11_add_module(tempmonitor src/TempMonitor.cc
                    src/base/TempMonitorBase.cc
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/messages/TempData.capnp.c++
                    include/base/TempMonitorBase.h
                    include/TempMonitor.h
        )
target_link_libraries(tempmonitor PRIVATE czmq riaps dl capnp kj)
set_target_properties(tempmonitor PROPERTIES PREFIX lib SUFFIX .so)



