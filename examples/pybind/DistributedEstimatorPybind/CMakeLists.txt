include_directories(include)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#add_custom_command(
#        OUTPUT  "include/messages/distributedestimator.capnp.c++"
#        DEPENDS "include/messages/capnp/distributedestimator.capnp"
#        WORKING_DIRECTORY "include/messages/capnp"
#        COMMAND /opt/riaps/amd64/bin/capnp compile distributedestimator.capnp -oc++:../
#        COMMENT "=== Generating capnp ==="
#)

include_directories(/usr/local/lib/python3.8/dist-packages/pybind11/include/)

add_library(globalestimator src/GlobalEstimator.cc
        src/base/GlobalEstimatorBase.cc
        include/messages/distributedestimator.capnp.c++
        include/base/GlobalEstimatorBase.h
        include/GlobalEstimator.h
                                    )
target_link_libraries(globalestimator PRIVATE czmq riaps dl capnp kj)
set_target_properties(globalestimator PROPERTIES PREFIX lib SUFFIX .so)

add_library(localestimator src/LocalEstimator.cc
        src/base/LocalEstimatorBase.cc
        include/messages/distributedestimator.capnp.c++
        include/base/LocalEstimatorBase.h
        include/LocalEstimator.h
        )
target_link_libraries(localestimator PRIVATE czmq riaps dl capnp kj)
set_target_properties(localestimator PROPERTIES PREFIX lib SUFFIX .so)

add_library(sensor src/Sensor.cc
        src/base/SensorBase.cc
        include/messages/distributedestimator.capnp.c++
        include/base/SensorBase.h
        include/Sensor.h
        )
target_link_libraries(sensor PRIVATE czmq riaps dl capnp kj)
set_target_properties(sensor PROPERTIES PREFIX lib SUFFIX .so)
