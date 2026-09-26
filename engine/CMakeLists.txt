cmake_minimum_required(VERSION 3.20)
project(PokoEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine build options
option(ENGINE_BUILD_TESTS "Build engine tests" ON)
option(ENGINE_ENABLE_PROFILING "Enable engine profiling" ON)
option(ENGINE_ENABLE_RENDERING "Enable rendering subsystem" ON)
option(ENGINE_ENABLE_PHYSICS "Enable physics subsystem" ON)
option(ENGINE_ENABLE_AUDIO "Enable audio subsystem" ON)

# Engine source directories
set(ENGINE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform)

# Source files - Core logging
file(GLOB_RECURSE ENGINE_LOGGING_SOURCES
    "${ENGINE_SOURCE_DIR}/core/logging/*.cpp"
    "${ENGINE_SOURCE_DIR}/core/logging/*.cc"
)

file(GLOB_RECURSE ENGINE_LOGGING_HEADERS
    "${ENGINE_INCLUDE_DIR}/core/logging/*.h"
    "${ENGINE_INCLUDE_DIR}/core/logging/*.hpp"
)

# Source files - Core error handling
file(GLOB ENGINE_ERROR_SOURCES
    "${ENGINE_SOURCE_DIR}/core/error.cpp"
    "${ENGINE_SOURCE_DIR}/core/error.cc"
)

file(GLOB ENGINE_ERROR_HEADERS
    "${ENGINE_INCLUDE_DIR}/core/error.h"
    "${ENGINE_INCLUDE_DIR}/core/error.hpp"
)

# Source files - Core serialization
file(GLOB ENGINE_SERIALIZATION_SOURCES
    "${ENGINE_SOURCE_DIR}/core/serialization/*.cpp"
    "${ENGINE_SOURCE_DIR}/core/serialization/*.cc"
)

file(GLOB ENGINE_SERIALIZATION_HEADERS
    "${ENGINE_INCLUDE_DIR}/core/serialization/*.h"
    "${ENGINE_INCLUDE_DIR}/core/serialization/*.hpp"
)

# Source files - Core handles
file(GLOB ENGINE_HANDLES_SOURCES
    "${ENGINE_SOURCE_DIR}/core/handles/*.cpp"
    "${ENGINE_SOURCE_DIR}/core/handles/*.cc"
)

file(GLOB ENGINE_HANDLES_HEADERS
    "${ENGINE_INCLUDE_DIR}/core/handles/*.h"
    "${ENGINE_INCLUDE_DIR}/core/handles/*.hpp"
)

# Source files - Core memory
file(GLOB ENGINE_MEMORY_SOURCES
    "${ENGINE_SOURCE_DIR}/core/memory/*.cpp"
    "${ENGINE_SOURCE_DIR}/core/memory/*.cc"
)

file(GLOB ENGINE_MEMORY_HEADERS
    "${ENGINE_INCLUDE_DIR}/core/memory/*.h"
    "${ENGINE_INCLUDE_DIR}/core/memory/*.hpp"
)

# Source files - Core (other modules - placeholders for now)
file(GLOB ENGINE_CORE_PLACEHOLDERS
    "${ENGINE_SOURCE_DIR}/core/*/placeholder.cpp"
)

# Source files - Platform
file(GLOB ENGINE_PLATFORM_SOURCES
    "${ENGINE_SOURCE_DIR}/platform/*.cpp"
    "${ENGINE_SOURCE_DIR}/platform/*.cc"
)

file(GLOB ENGINE_PLATFORM_HEADERS
    "${ENGINE_INCLUDE_DIR}/platform/*.h"
    "${ENGINE_INCLUDE_DIR}/platform/*.hpp"
)

# Core engine library
add_library(poko_core STATIC
    ${ENGINE_LOGGING_SOURCES}
    ${ENGINE_LOGGING_HEADERS}
    ${ENGINE_ERROR_SOURCES}
    ${ENGINE_ERROR_HEADERS}
    ${ENGINE_SERIALIZATION_SOURCES}
    ${ENGINE_SERIALIZATION_HEADERS}
    ${ENGINE_HANDLES_SOURCES}
    ${ENGINE_HANDLES_HEADERS}
    ${ENGINE_MEMORY_SOURCES}
    ${ENGINE_MEMORY_HEADERS}
    ${ENGINE_CORE_PLACEHOLDERS}
)

target_include_directories(poko_core PUBLIC
    $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(poko_core PUBLIC cxx_std_20)

# Platform abstraction library (only if sources exist)
if(ENGINE_PLATFORM_SOURCES OR ENGINE_PLATFORM_HEADERS)
    add_library(poko_platform STATIC
        ${ENGINE_PLATFORM_SOURCES}
        ${ENGINE_PLATFORM_HEADERS}
    )

    target_include_directories(poko_platform PUBLIC
        $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(poko_platform PUBLIC poko_core)

    # Compiler warnings
    if(MSVC)
        target_compile_options(poko_platform PRIVATE /W4)
    else()
        target_compile_options(poko_platform PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Platform-specific configurations
    if(WIN32)
        # Windows-specific settings
        target_compile_definitions(poko_platform PRIVATE PLATFORM_WINDOWS)
    elseif(ANDROID)
        # Android-specific settings
        target_compile_definitions(poko_platform PRIVATE PLATFORM_ANDROID)
    elseif(UNIX)
        # Unix-specific settings
        target_compile_definitions(poko_platform PRIVATE PLATFORM_UNIX)
    endif()
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(poko_core PRIVATE /W4)
else()
    target_compile_options(poko_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS poko_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(ENGINE_PLATFORM_SOURCES OR ENGINE_PLATFORM_HEADERS)
    install(TARGETS poko_platform
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Tests
if(ENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
