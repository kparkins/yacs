cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(YACS_VERSION_REGEX "#define YACS_VERSION_.*[ \t]+(.+)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h" YACS_VERSION REGEX ${YACS_VERSION_REGEX})
list(TRANSFORM YACS_VERSION REPLACE ${YACS_VERSION_REGEX} "\\1")
string(JOIN "." YACS_VERSION ${YACS_VERSION})

project(
    yacs
    VERSION ${YACS_VERSION}
    DESCRIPTION "Yet Another Component System - another day another Yak shaved"
    LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(yacs INTERFACE)

target_include_directories(
    yacs
    INTERFACE
        $<BUILD_INTERFACE:${yacs_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${yacs_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(yacs INTERFACE cxx_std_17)

target_sources(
    yacs 
    INTERFACE
        $<BUILD_INTERFACE:${yacs_SOURCE_DIR}/include/pool.h>
)

option(YACS_BUILD_TESTING "Enable building tests." ON)

if(YACS_BUILD_TESTING)
    set(YACS_CXX_STD cxx_std_17 CACHE STRING "C++ standard revision to use for the tests")

    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
