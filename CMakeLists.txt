cmake_minimum_required(VERSION 3.0.0)
project(riaps_core VERSION 0.6.0)
option(arch "amd64/armhf" "amd64")
option(BUILD_TESTS  "Build tests" ON)
set(CMAKE_SYSTEM_NAME Linux)

set(riaps_prefix "/opt/riaps/" CACHE STRING "the riaps prefix")





#Set the platform
if (${arch} STREQUAL "armhf")
 set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)
 set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
 set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
 set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
 set (CMAKE_CXX_FLAGS "-std=c++11 -g")
 set (CMAKE_C_FLAGS "-std=c99 -g")
else()
 set(CMAKE_C_COMPILER gcc-5)
 set(CMAKE_CXX_COMPILER g++-5)
 set (CMAKE_CXX_FLAGS "-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0")
 set (CMAKE_C_FLAGS "-std=c99")
endif()


set(CMAKE_INSTALL_PREFIX ${riaps_prefix}/${arch})

set(DEPENDENCIES ${riaps_prefix})
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
include_directories(${LIBALLPATH_INCLUDE})
link_directories(${LIBALLPATH_LIB})


# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

#add includes
include_directories(include)
#add_subdirectory(src/componentmodel)
#add_subdirectory(src/discoveryd)
#add_subdirectory(src/devm)
#add_subdirectory(src/device)

if (BUILD_TESTS)
    include(CTest)
    enable_testing()
    # Tests
    # add_subdirectory(examples/DistributedEstimator)
    # add_subdirectory(examples/DistributedEstimatorGroup)
    #add_subdirectory(tests/test_timer)
    #add_subdirectory(tests/GpioDeviceTesting)
    #add_subdirectory(tests/testPubSub/test_1_1/test_1_1)
    #add_subdirectory(tests/EchoIO)
endif()

# libriaps sources
list (APPEND component_SOURCE
        include/messaging/disco.capnp.c++
        include/messaging/distcoord.capnp.c++
        src/componentmodel/r_actor.cc
        src/componentmodel/r_argumentparser.cc
        src/componentmodel/r_commandlineparser.cc
        src/componentmodel/r_componentbase.cc
        src/componentmodel/r_devicethread.cc
        src/componentmodel/r_devmapi.cc
        src/componentmodel/r_discoverdapi.cc
        src/componentmodel/r_insideport.cc
        src/componentmodel/r_messagebase.cc
        src/componentmodel/r_parameter.cc
        src/componentmodel/r_periodictimer.cc
        src/componentmodel/r_peripheral.cc
        src/componentmodel/r_portbase.cc
        src/componentmodel/r_portregister.cc
        src/componentmodel/r_publisherport.cc
        src/componentmodel/r_pubportbase.cc
        src/componentmodel/r_pubportgroup.cc
        src/componentmodel/r_requestport.cc
        src/componentmodel/r_responseport.cc
        src/componentmodel/r_senderport.cc
        src/componentmodel/r_subportbase.cc
        src/componentmodel/r_subportgroup.cc
        src/componentmodel/r_subscriberport.cc
        src/framework/rfw_configuration.cc
        src/framework/rfw_network_interfaces.cc
        src/groups/r_group.cc
        src/messaging/r_message.cc
        src/utils/r_capnp_helper.cc
        src/utils/r_message.cc
        src/utils/r_utils.cc
        )

# libriaps headers
list (APPEND component_HEADERS
        include/componentmodel/r_actor.h
        include/componentmodel/r_commandlineparser.h
        include/componentmodel/r_componentbase.h
        include/componentmodel/r_configuration.h
        include/componentmodel/r_devicethread.h
        include/componentmodel/r_discoverdapi.h
        include/componentmodel/r_discoverdapi.h
        include/componentmodel/r_insideport.h
        include/componentmodel/r_messagebase.h
        include/componentmodel/r_parameter.h
        include/componentmodel/r_periodictimer.h
        include/componentmodel/r_portbase.h
        include/componentmodel/r_portregister.h
        include/componentmodel/r_publisherport.h
        include/componentmodel/r_pubportbase.h
        include/componentmodel/r_pubportgroup.h
        include/componentmodel/r_requestport.h
        include/componentmodel/r_responseport.h
        include/componentmodel/r_senderport.h
        include/componentmodel/r_subportbase.h
        include/componentmodel/r_subportgroup.h
        include/componentmodel/r_subscriberport.h
        include/const/r_const.h
        include/groups/r_group.h
        include/messaging/disco.capnp.h
        include/messaging/distcoord.capnp.h
        include/utils/r_expirable.h
        include/utils/r_expirablelist.h
        )

# libriaps
add_library(riaps SHARED
        ${component_SOURCE}
        ${component_HEADERS}
        )
target_link_libraries(riaps czmq zmq dl capnp kj)
install(TARGETS riaps DESTINATION lib)

# start_actor
set(ACTOR_SOURCE
        src/componentmodel/r_actor_main.cc
        )

set(ACTOR_HEADERS
        include/componentmodel/r_actor.h
        include/componentmodel/r_argumentparser.h
        include/componentmodel/r_actor_main.h
        include/groups/r_group.h
        include/const/r_jsonmodel.h
        include/const/r_const.h
        include/messaging/disco.capnp.h
        include/json.h
        )

add_executable(start_actor
        ${ACTOR_SOURCE}
        ${ACTOR_HEADERS}
        )
target_link_libraries(start_actor czmq zmq riaps capnp)
install(TARGETS start_actor DESTINATION bin)

# rdiscoveryd source
list(APPEND RDISCOVERYD_SOURCE
        src/discoveryd/r_discoveryd.cc
        src/discoveryd/r_riaps_actor.cc
        src/discoveryd/r_msghandler.cc
        include/messaging/disco.capnp.c++
        )

# rdiscoveryd headers
list(APPEND RDISCOVERYD_HEADERS
        include/discoveryd/r_riaps_actor.h
        include/discoveryd/r_discovery_types.h
        include/discoveryd/r_msghandler.h
        include/discoveryd/r_registration.h
        include/framework/rfw_configuration.h
        include/framework/rfw_network_interfaces.h
        include/utils/r_utils.h
        include/messaging/disco.capnp.h
        include/groups/r_group.h
        include/spdlog/spdlog.h
        )

# rdiscoveryd
add_executable(rdiscoveryd
        ${RDISCOVERYD_SOURCE}
        ${RDISCOVERYD_HEADERS}
        )
target_link_libraries(rdiscoveryd czmq zmq opendht gnutls riaps capnp nettle hogweed gmp pthread)

install(TARGETS rdiscoveryd DESTINATION bin)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "6")
SET(PATCH_VERSION "0")

IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
    INCLUDE(InstallRequiredSystemLibraries)
    SET(CPACK_SET_DESTDIR "on")
    SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_NAME "riapscore-${arch}")
    SET(CPACK_PACKAGE_DESCRIPTION "RiapsDiscoveryService")
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This is the decentralized discovery service for RIAPS")
    SET(CPACK_PACKAGE_VENDOR "riaps")
    SET(CPACK_PACKAGE_CONTACT "riaps@vanderbilt.edu")
    SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
    SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
    SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
    SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "riaps-externals-${arch}")
    SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${arch})
    SET(CPACK_COMPONENTS_ALL Libraries ApplicationData)
    INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

install(DIRECTORY include/ DESTINATION include
        PATTERN "boost" EXCLUDE)
