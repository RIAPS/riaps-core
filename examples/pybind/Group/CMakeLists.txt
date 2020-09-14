include_directories(include)

# riaps:keep_cmake:begin

# riaps:keep_cmake:end


add_custom_command(
        OUTPUT  "${CMAKE_SOURCE_DIR}/include/messages/groupapp.capnp.c++"
        DEPENDS "${CMAKE_SOURCE_DIR}/groupapp.capnp"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND capnp compile ./groupapp.capnp -oc++:./include/messages/
        COMMENT "=== Generating capnp ==="
)

# riaps:keep_groupiecpp:begin
add_library(groupiecpp SHARED
        src/GroupieCpp.cc
        src/base/GroupieCppBase.cc
        include/base/GroupieCppBase.h
        include/GroupieCpp.h
        include/messages/groupapp.capnp.c++
        )
target_link_libraries(groupiecpp PRIVATE czmq riaps dl capnp kj)
set_target_properties(groupiecpp PROPERTIES PREFIX lib SUFFIX .so)
# riaps:keep_groupiecpp:end




