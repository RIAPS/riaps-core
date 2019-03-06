
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(/usr/include/python3.6m/)
include_directories(include)
# riaps:keep_cmake:begin

# riaps:keep_cmake:end


add_custom_command(
        OUTPUT  "${CMAKE_SOURCE_DIR}/include/messages/timersync.capnp.c++"
        DEPENDS "${CMAKE_SOURCE_DIR}/timersync.capnp"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND capnp compile ./timersync.capnp -oc++:./include/messages/
        COMMENT "=== Generating capnp ==="
)

# riaps:keep_timercomp:begin
add_library(timercomp SHARED
        src/TimerComp.cc
        src/base/TimerCompBase.cc
        include/base/TimerCompBase.h
        include/TimerComp.h
        include/messages/timersync.capnp.c++
        )
target_link_libraries(timercomp PRIVATE czmq riaps dl capnp kj)
set_target_properties(timercomp PROPERTIES PREFIX lib SUFFIX .so)
# riaps:keep_timercomp:end




