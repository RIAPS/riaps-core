include_directories(include)

add_custom_command(
		OUTPUT  include/messages/TempData.capnp.c++ include/messages/TempData.capnp.h
		DEPENDS include/messages/TempData.capnp
		COMMAND capnp compile -oc++ include/messages/TempData.capnp
		VERBATIM
)


add_library(tempsensor SHARED src/TempSensor.cc
		                                  src/base/TempSensorBase.cc
		                                  include/messages/TempData.capnp.c++
)

add_library(tempmonitor SHARED src/TempMonitor.cc
		                                  src/base/TempMonitorBase.cc
		                                  include/messages/TempData.capnp.c++
)


target_link_libraries(tempsensor czmq riaps dl capnp kj )

target_link_libraries(tempmonitor czmq riaps dl capnp kj )

