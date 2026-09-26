cmake_minimum_required(VERSION 3.20)
project(Poko VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Poko project options
option(POKO_BUILD_ENGINE "Build Poko Engine" ON)
option(POKO_BUILD_TESTS "Build Poko tests" ON)
option(POKO_BUILD_EXAMPLES "Build Poko examples" ON)
option(POKO_ENABLE_PROFILING "Enable profiling support" ON)
option(POKO_ANDROID_TARGET "Android target build" OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Poko-specific directories
set(POKO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POKO_SOURCE_DIR ${POKO_ROOT_DIR})
set(POKO_INCLUDE_DIR ${POKO_ROOT_DIR}/include)
set(POKO_ENGINE_DIR ${POKO_ROOT_DIR}/engine)
set(POKO_TOOLING_DIR ${POKO_ROOT_DIR}/tooling)

# Compiler warnings and standards
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include Mute submodule
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mute/CMakeLists.txt)
    # TODO: Add mute sources before enabling this
    # add_subdirectory(mute)
    message(STATUS "Mute submodule found but not yet integrated")
else()
    message(WARNING "Mute submodule not found. Initialize with: git submodule update --init")
endif()

# Build Poko Engine
if(POKO_BUILD_ENGINE)
    add_subdirectory(engine)
endif()

# Build tooling
add_subdirectory(tooling)

# Tests
if(POKO_BUILD_TESTS)
    enable_testing()
    # TODO: Add test subdirectories when available
endif()

# Examples
if(POKO_BUILD_EXAMPLES)
    # TODO: Add example subdirectories when available
endif()

# Installation rules
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Summary
message(STATUS "Poko Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
