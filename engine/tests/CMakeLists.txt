# Poko Engine Tests
cmake_minimum_required(VERSION 3.15)

project(poko_tests)

# Find testing infrastructure
set(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# Test sources
file(GLOB_RECURSE TEST_SOURCES
    "${TEST_SOURCE_DIR}/*.cpp"
)

# Test executable
add_executable(poko_test_runner
    ${TEST_SOURCES}
    ../src/core/testing/test.cpp
)

target_link_libraries(poko_test_runner
    PRIVATE
    poko_core
)

target_include_directories(poko_test_runner PRIVATE
    ${TEST_INCLUDE_DIR}
)

target_compile_features(poko_test_runner PRIVATE cxx_std_20)

# Enable testing
enable_testing()

# Add test
add_test(NAME AllTests COMMAND poko_test_runner)
