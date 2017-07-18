cmake_minimum_required(VERSION 3.0.0)
project(riaps_core VERSION 0.4.9)
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
 set (CMAKE_CXX_FLAGS "-std=c++11")
 set (CMAKE_C_FLAGS "-std=c99")
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
add_subdirectory(src/componentmodel)
add_subdirectory(src/discoveryd)
add_subdirectory(src/devm)
add_subdirectory(src/device)

if (BUILD_TESTS)
    include(CTest)
    enable_testing()
    # Tests
    #add_subdirectory(tests/test_distributed_estimator_cmessage_nowrap)
    #add_subdirectory(tests/test_timer)
    #add_subdirectory(tests/GpioDeviceTesting)
    #add_subdirectory(tests/testPubSub/test_1_1/test_1_1)
    #add_subdirectory(tests/EchoIO)
endif()






SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "4")
SET(PATCH_VERSION "9")

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
