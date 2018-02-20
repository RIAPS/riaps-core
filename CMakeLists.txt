cmake_minimum_required(VERSION 3.0.0)
project(riaps_core VERSION 0.6.3)
option(arch "amd64/armhf" "amd64")
option(BUILD_TESTS  "Build tests" ON)
set(CMAKE_SYSTEM_NAME Linux)

set(riaps_prefix "/opt/riaps/" CACHE STRING "the riaps prefix")





#Set the platform
if (${arch} STREQUAL "armhf")
 set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)
 set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
 set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-5)
 set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
 set (CMAKE_CXX_FLAGS "-std=c++11 -g")
 set (CMAKE_C_FLAGS "-std=c99 -g")
else()
 set(CMAKE_C_COMPILER gcc-5)
 set(CMAKE_CXX_COMPILER g++-5)
 set (CMAKE_CXX_FLAGS "-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0")
 set (CMAKE_C_FLAGS "-std=c99")
endif()

#set(pybind11_DIR ${CMAKE_HOME_DIRECTORY})

set(CMAKE_INSTALL_PREFIX ${riaps_prefix}/${arch})

set(DEPENDENCIES ${riaps_prefix})
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
include_directories(${LIBALLPATH_INCLUDE})
link_directories(${LIBALLPATH_LIB})

#set(PYTHON_EXECUTABLE /usr/bin/python3)


# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

include_directories(include)

if (BUILD_TESTS)
    include(CTest)
    enable_testing()
    # Tests
    # add_subdirectory(examples/DistributedEstimatorPy)
    #add_subdirectory(examples/DistributedEstimator)
    #add_subdirectory(examples/DistributedEstimatorGroup)
    #add_subdirectory(examples/LeaderElection)
    #add_subdirectory(examples/Consensus)
    #add_subdirectory(examples/ActiveReplica)
    #add_subdirectory(examples/AdaptiveTimer)
    #add_subdirectory(examples/TSyncCA)
    #add_subdirectory(tests/StaticTimer)
    #add_subdirectory(tests/test_timer)
    #add_subdirectory(tests/GpioDeviceTesting)
    #add_subdirectory(tests/testPubSub/test_1_1/test_1_1)
    #add_subdirectory(tests/ScheduledTimer)

    # add_subdirectory(examples/EchoIO)
endif()


# libriaps sources
list (APPEND component_SOURCE
        include/messaging/disco.capnp.c++
        include/messaging/distcoord.capnp.c++
        src/componentmodel/ports/r_queryport.cc
        src/componentmodel/ports/r_answerport.cc
        src/componentmodel/ports/r_insideport.cc
        src/componentmodel/ports/r_portbase.cc
        src/componentmodel/ports/r_publisherport.cc
        src/componentmodel/ports/r_pubportbase.cc
        src/componentmodel/ports/r_pubportgroup.cc
        src/componentmodel/ports/r_requestport.cc
        src/componentmodel/ports/r_responseport.cc
        src/componentmodel/ports/r_senderport.cc
        src/componentmodel/ports/r_subportbase.cc
        src/componentmodel/ports/r_subportgroup.cc
        src/componentmodel/ports/r_subscriberport.cc
        src/componentmodel/r_actor.cc
        src/componentmodel/r_argumentparser.cc
        src/componentmodel/r_payload.cc
        src/componentmodel/r_commandlineparser.cc
        src/componentmodel/r_componentbase.cc
        src/componentmodel/r_devicethread.cc
        src/componentmodel/r_devmapi.cc
        src/componentmodel/r_discoverdapi.cc
        src/componentmodel/r_messagebase.cc
        src/componentmodel/r_parameter.cc
        src/componentmodel/r_periodictimer.cc
        src/componentmodel/r_peripheral.cc
        src/componentmodel/r_portregister.cc
        src/framework/rfw_configuration.cc
        src/framework/rfw_network_interfaces.cc
        src/groups/r_group.cc
        src/groups/r_grouplead.cc
        src/utils/r_message.cc
        src/utils/r_utils.cc
        src/utils/r_timeout.cc
        )

# libriaps headers
list (APPEND component_HEADERS
        include/componentmodel/ports/r_queryport.h
        include/componentmodel/ports/r_answerport.h
        include/componentmodel/ports/r_insideport.h
        include/componentmodel/ports/r_portbase.h
        include/componentmodel/ports/r_publisherport.h
        include/componentmodel/ports/r_pubportbase.h
        include/componentmodel/ports/r_pubportgroup.h
        include/componentmodel/ports/r_requestport.h
        include/componentmodel/ports/r_responseport.h
        include/componentmodel/ports/r_senderport.h
        include/componentmodel/ports/r_subportbase.h
        include/componentmodel/ports/r_subportgroup.h
        include/componentmodel/ports/r_subscriberport.h
        include/componentmodel/r_actor.h
        include/componentmodel/r_payload.h
        include/componentmodel/r_commandlineparser.h
        include/componentmodel/r_componentbase.h
        include/componentmodel/r_configuration.h
        include/componentmodel/r_devicethread.h
        include/componentmodel/r_discoverdapi.h
        include/componentmodel/r_discoverdapi.h
        include/componentmodel/r_messagebase.h
        include/componentmodel/r_parameter.h
        include/componentmodel/r_periodictimer.h
        include/componentmodel/r_portregister.h
        include/componentmodel/r_riapsmessage.h
        include/const/r_const.h
        include/groups/r_group.h
        include/groups/r_grouplead.h
        include/messaging/disco.capnp.h
        include/messaging/distcoord.capnp.h
        include/utils/r_expirable.h
        include/utils/r_expirablelist.h
        include/utils/r_timeout.h
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
        include/json.h)

add_executable(start_actor
        ${ACTOR_SOURCE}
        ${ACTOR_HEADERS}
        )
target_link_libraries(start_actor czmq zmq riaps capnp pthread)
install(TARGETS start_actor DESTINATION bin)

# rdiscoveryd source
list(APPEND RDISCOVERYD_SOURCE
        src/discoveryd/r_discoveryd.cc
        src/discoveryd/r_riaps_actor.cc
        src/discoveryd/r_msghandler.cc
        include/messaging/disco.capnp.c++
        src/utils/r_timeout.cc
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
        include/utils/r_timeout.h
        )

# rdiscoveryd
add_executable(rdiscoveryd
        ${RDISCOVERYD_SOURCE}
        ${RDISCOVERYD_HEADERS}
        )
target_link_libraries(rdiscoveryd czmq zmq opendht gnutls riaps capnp nettle hogweed gmp pthread)

install(TARGETS rdiscoveryd DESTINATION bin)


# start_device source
list(APPEND DEVICE_SOURCE
        src/device/r_deviceactor.cc
        src/device/r_dmain.cc
        )

# start_device headers
list(APPEND DEVICE_HEADERS
        include/device/r_deviceactor.h
        )

# start_device
add_executable(start_device
        ${DEVICE_SOURCE}
        ${DEVICE_HEADERS}
        )
target_link_libraries(start_device czmq zmq opendht gnutls riaps capnp nettle hogweed gmp pthread)

install(TARGETS start_device DESTINATION bin)


SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "6")
SET(PATCH_VERSION "3")

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

install(DIRECTORY include/ DESTINATION include)
        #PATTERN "boost" EXCLUDE)
