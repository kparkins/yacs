include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    GIT_SHALLOW 1
)

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

add_library(GTest::Main ALIAS gtest_main)

target_compile_features(gtest PUBLIC cxx_std_17)
target_compile_features(gtest_main PUBLIC cxx_std_17)
target_compile_features(gmock PUBLIC cxx_std_17)
target_compile_features(gmock_main PUBLIC cxx_std_17)

function(SETUP_TARGET TARGET_NAME)
    target_compile_features(${TARGET_NAME} PRIVATE ${YACS_CXX_STD})
    target_link_libraries(${TARGET_NAME} PRIVATE yacs)

    if(MSVC)
        target_compile_options(
            ${TARGET_NAME}
            PRIVATE
                /EHsc /W4 /wd4996 /w14800 /wd4100 /wd4201 /wd4127
                $<$<CONFIG:Debug>:/Od>
                $<$<CONFIG:Release>:/O2>
        )
    else()
        target_compile_options(
            ${TARGET_NAME}
            PRIVATE
                -Wall -Wshadow -Wno-deprecated-declarations
                $<$<CONFIG:Debug>:-O0 -g>
                $<$<CONFIG:Release>:-O2>
        )
    endif()
endfunction()

function(SETUP_TEST TEST_NAME TEST_SOURCES)
    add_executable(${TEST_NAME} ${TEST_SOURCES})
    target_link_libraries(${TEST_NAME} PRIVATE GTest::Main)
    SETUP_TARGET(${TEST_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

SETUP_TEST(pool pool.cpp data_struct.hpp)
SETUP_TEST(component component.cpp data_struct.hpp)
SETUP_TEST(registry registry.cpp data_struct.hpp)