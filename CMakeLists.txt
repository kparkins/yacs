cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Setup version file
set(YACS_VERSION_REGEX "#define YACS_VERSION_.*[ \t]+(.+)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h" YACS_VERSION REGEX ${YACS_VERSION_REGEX})
list(TRANSFORM YACS_VERSION REPLACE ${YACS_VERSION_REGEX} "\\1")
string(JOIN "." YACS_VERSION ${YACS_VERSION})

# Setup project
project(
    yacs
    VERSION ${YACS_VERSION}
    DESCRIPTION "Yet Another Component System - another day another Yak shaved"
    LANGUAGES C CXX
)

# Setup build information
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(YACS_USE_SANITIZER "Enable address sanitizer by adding -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer" ON)

if(YACS_USE_SANITIZER)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(YACS_HAS_SANITIZER TRUE CACHE BOOL "" FORCE)
    endif()

    if(NOT YACS_HAS_SANITIZER)
        message(VERBOSE "Option to use AddressSanitizer is set but support is not available. Not adding flags.")
    endif()
endif()

# Setup yacs library
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
        $<BUILD_INTERFACE:${yacs_SOURCE_DIR}/include/pool.hpp>
)

if(YACS_HAS_SANITIZER)
    target_compile_options(yacs INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer>)
    target_link_libraries(yacs INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer>)
endif()

option(YACS_BUILD_TESTING "Enable building tests." ON)

# Setup Testing
if(YACS_BUILD_TESTING)
    set(YACS_CXX_STD cxx_std_17 CACHE STRING "C++ standard for tests.")

    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
