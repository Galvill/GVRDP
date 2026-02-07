find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
include(GoogleTest)

# Test: connection profile JSON serialization
add_executable(test_connection_profile
    test_connection_profile.cpp
    ${CMAKE_SOURCE_DIR}/src/config/connection_profile.cpp
)
target_include_directories(test_connection_profile PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_connection_profile PRIVATE
    GTest::gtest GTest::gtest_main
    nlohmann_json::nlohmann_json
)
gtest_discover_tests(test_connection_profile)

# Test: debouncer
add_executable(test_debouncer
    test_debouncer.cpp
    ${CMAKE_SOURCE_DIR}/src/util/debouncer.cpp
)
target_include_directories(test_debouncer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_debouncer PRIVATE
    GTest::gtest GTest::gtest_main
)
gtest_discover_tests(test_debouncer)

# Test: keyboard map
add_executable(test_keyboard_map
    test_keyboard_map.cpp
    ${CMAKE_SOURCE_DIR}/src/input/keyboard_map.cpp
)
target_include_directories(test_keyboard_map PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_keyboard_map PRIVATE
    GTest::gtest GTest::gtest_main
    SDL2::SDL2
    PkgConfig::FREERDP3
)
gtest_discover_tests(test_keyboard_map)
