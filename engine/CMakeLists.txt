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

# External dependencies
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external")

# Diligent Engine - temporarily disabled due to submodule initialization issues
# Will be re-enabled when build system is properly configured
set(DILIGENT_DIR "${EXTERNAL_DIR}/diligent")
set(DILIGENT_CORE_DIR "${EXTERNAL_DIR}/diligentcore")

# if(EXISTS "${DILIGENT_DIR}" AND EXISTS "${DILIGENT_CORE_DIR}")
#     message(STATUS "Diligent Engine found at ${DILIGENT_DIR}")
#     message(STATUS "Diligent Core found at ${DILIGENT_CORE_DIR}")
#     
#     set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
#     set(DILIGENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#     set(DILIGENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
#     set(DILIGENT_USE_VULKAN ON CACHE BOOL "" FORCE)
#     set(DILIGENT_USE_GL OFF CACHE BOOL "" FORCE)
#     set(DILIGENT_SKIP_SPIRV_TOOLS ON CACHE BOOL "" FORCE)
#     set(DILIGENT_SKIP_GLSLANG ON CACHE BOOL "" FORCE)
#     set(DILIGENT_SKIP_SPIRV_CROSS ON CACHE BOOL "" FORCE)
#     set(DILIGENT_SKIP_VULKAN_HEADERS ON CACHE BOOL "" FORCE)
#     
#     add_subdirectory(${DILIGENT_CORE_DIR} ${CMAKE_BINARY_DIR}/diligentcore EXCLUDE_FROM_ALL)
#     add_subdirectory(${DILIGENT_DIR} ${CMAKE_BINARY_DIR}/diligent EXCLUDE_FROM_ALL)
#     
#     target_compile_definitions(poko_core PUBLIC ENGINE_ENABLE_DILIGENT)
# else()
    message(WARNING "Diligent Engine integration temporarily disabled - renderer will use abstraction layer")
# endif()

# Jolt Physics
set(JOLT_DIR "${EXTERNAL_DIR}/jolt")
set(JOLT_INCLUDE_DIR "${JOLT_DIR}/Jolt")
set(JOLT_LIBRARY_DIR "${JOLT_DIR}/Build/MinGW_Debug")
set(JOLT_LIBRARY "${JOLT_LIBRARY_DIR}/libJolt.a")

if(EXISTS "${JOLT_DIR}" AND EXISTS "${JOLT_LIBRARY}")
    message(STATUS "Jolt Physics found at ${JOLT_DIR}")
    message(STATUS "Jolt Library: ${JOLT_LIBRARY}")
    include_directories(${JOLT_INCLUDE_DIR})
else()
    message(WARNING "Jolt Physics not found or not built - physics will be stub")
    set(JOLT_LIBRARY "")
endif()

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

# Source files - Runtime
file(GLOB_RECURSE ENGINE_RUNTIME_SOURCES
    "${ENGINE_SOURCE_DIR}/runtime/*.cpp"
    "${ENGINE_SOURCE_DIR}/runtime/*.cc"
)

file(GLOB_RECURSE ENGINE_RUNTIME_HEADERS
    "${ENGINE_INCLUDE_DIR}/runtime/*.h"
    "${ENGINE_INCLUDE_DIR}/runtime/*.hpp"
)

# Source files - Binding
file(GLOB ENGINE_BINDING_SOURCES
    "${ENGINE_SOURCE_DIR}/binding/*.cpp"
    "${ENGINE_SOURCE_DIR}/binding/*.cc"
)

file(GLOB ENGINE_BINDING_HEADERS
    "${ENGINE_INCLUDE_DIR}/binding/*.h"
    "${ENGINE_INCLUDE_DIR}/binding/*.hpp"
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

# Source files - Renderer
file(GLOB_RECURSE ENGINE_RENDERER_SOURCES
    "${ENGINE_SOURCE_DIR}/renderer/*.cpp"
    "${ENGINE_SOURCE_DIR}/renderer/*.cc"
)

file(GLOB_RECURSE ENGINE_RENDERER_HEADERS
    "${ENGINE_INCLUDE_DIR}/renderer/*.h"
    "${ENGINE_INCLUDE_DIR}/renderer/*.hpp"
)

# Source files - Physics
if(EXISTS "${JOLT_LIBRARY}")
    # Use Jolt physics adapter
    set(ENGINE_PHYSICS_SOURCES
        "${ENGINE_SOURCE_DIR}/physics/jolt/jolt_adapter.cpp"
    )
    message(STATUS "Using Jolt Physics adapter")
else()
    # Use placeholder physics
    set(ENGINE_PHYSICS_SOURCES
        "${ENGINE_SOURCE_DIR}/physics/physics_placeholder.cpp"
    )
    message(STATUS "Using placeholder physics")
endif()

file(GLOB_RECURSE ENGINE_PHYSICS_HEADERS
    "${ENGINE_INCLUDE_DIR}/physics/*.h"
    "${ENGINE_INCLUDE_DIR}/physics/*.hpp"
)

# Source files - Audio
file(GLOB_RECURSE ENGINE_AUDIO_SOURCES
    "${ENGINE_SOURCE_DIR}/audio/*.cpp"
    "${ENGINE_SOURCE_DIR}/audio/*.cc"
)

file(GLOB_RECURSE ENGINE_AUDIO_HEADERS
    "${ENGINE_INCLUDE_DIR}/audio/*.h"
    "${ENGINE_INCLUDE_DIR}/audio/*.hpp"
)

# Source files - Input
file(GLOB_RECURSE ENGINE_INPUT_SOURCES
    "${ENGINE_SOURCE_DIR}/input/*.cpp"
    "${ENGINE_SOURCE_DIR}/input/*.cc"
)

file(GLOB_RECURSE ENGINE_INPUT_HEADERS
    "${ENGINE_INCLUDE_DIR}/input/*.h"
    "${ENGINE_INCLUDE_DIR}/input/*.hpp"
)

# Source files - Camera
file(GLOB_RECURSE ENGINE_CAMERA_SOURCES
    "${ENGINE_SOURCE_DIR}/camera/*.cpp"
    "${ENGINE_SOURCE_DIR}/camera/*.cc"
)

file(GLOB_RECURSE ENGINE_CAMERA_HEADERS
    "${ENGINE_INCLUDE_DIR}/camera/*.h"
    "${ENGINE_INCLUDE_DIR}/camera/*.hpp"
)

# Source files - Animation
file(GLOB_RECURSE ENGINE_ANIMATION_SOURCES
    "${ENGINE_SOURCE_DIR}/animation/*.cpp"
    "${ENGINE_SOURCE_DIR}/animation/*.cc"
)

file(GLOB_RECURSE ENGINE_ANIMATION_HEADERS
    "${ENGINE_INCLUDE_DIR}/animation/*.h"
    "${ENGINE_INCLUDE_DIR}/animation/*.hpp"
)

# Source files - UI
file(GLOB_RECURSE ENGINE_UI_SOURCES
    "${ENGINE_SOURCE_DIR}/ui/*.cpp"
    "${ENGINE_SOURCE_DIR}/ui/*.cc"
)

file(GLOB_RECURSE ENGINE_UI_HEADERS
    "${ENGINE_INCLUDE_DIR}/ui/*.h"
    "${ENGINE_INCLUDE_DIR}/ui/*.hpp"
)

# Source files - Networking
file(GLOB_RECURSE ENGINE_NETWORKING_SOURCES
    "${ENGINE_SOURCE_DIR}/networking/*.cpp"
    "${ENGINE_SOURCE_DIR}/networking/*.cc"
)

file(GLOB_RECURSE ENGINE_NETWORKING_HEADERS
    "${ENGINE_INCLUDE_DIR}/networking/*.h"
    "${ENGINE_INCLUDE_DIR}/networking/*.hpp"
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
    ${ENGINE_RUNTIME_SOURCES}
    ${ENGINE_RUNTIME_HEADERS}
    ${ENGINE_BINDING_SOURCES}
    ${ENGINE_BINDING_HEADERS}
    ${ENGINE_RENDERER_SOURCES}
    ${ENGINE_RENDERER_HEADERS}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_PHYSICS_HEADERS}
    ${ENGINE_AUDIO_SOURCES}
    ${ENGINE_AUDIO_HEADERS}
    ${ENGINE_INPUT_SOURCES}
    ${ENGINE_INPUT_HEADERS}
    ${ENGINE_CAMERA_SOURCES}
    ${ENGINE_CAMERA_HEADERS}
    ${ENGINE_ANIMATION_SOURCES}
    ${ENGINE_ANIMATION_HEADERS}
    ${ENGINE_UI_SOURCES}
    ${ENGINE_UI_HEADERS}
    ${ENGINE_NETWORKING_SOURCES}
    ${ENGINE_NETWORKING_HEADERS}
    ${ENGINE_CORE_PLACEHOLDERS}
)

target_include_directories(poko_core PUBLIC
    $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(poko_core PUBLIC cxx_std_20)

# Link Windows libraries for audio support
if(WIN32)
    target_link_libraries(poko_core PUBLIC winmm)
endif()

# Miniaudio - for production audio
set(MINIAUDIO_DIR "${EXTERNAL_DIR}/miniaudio")
if(EXISTS "${MINIAUDIO_DIR}")
    message(STATUS "Miniaudio found at ${MINIAUDIO_DIR}")
    target_include_directories(poko_core PUBLIC "${MINIAUDIO_DIR}")
    target_compile_definitions(poko_core PUBLIC MA_NO_DEVICE_NOTIFICATION=1)
endif()

# Jolt Physics - for production physics
set(JOLT_DIR "${EXTERNAL_DIR}/jolt")
set(JOLT_LIBRARY "${JOLT_DIR}/Build/MinGW_Debug/libJolt.a")
if(EXISTS "${JOLT_DIR}" AND EXISTS "${JOLT_LIBRARY}")
    message(STATUS "Jolt Physics found at ${JOLT_DIR}")
    message(STATUS "Linking Jolt Physics library: ${JOLT_LIBRARY}")
    target_include_directories(poko_core PUBLIC "${JOLT_DIR}/Jolt")
    target_link_libraries(poko_core PUBLIC "${JOLT_LIBRARY}")
else()
    message(WARNING "Jolt Physics library not found at ${JOLT_LIBRARY}")
endif()

# libsodium - for networking/cryptography
set(SODIUM_DIR "${EXTERNAL_DIR}/libsodium")
if(EXISTS "${SODIUM_DIR}")
    message(STATUS "libsodium found at ${SODIUM_DIR}")
    target_include_directories(poko_core PUBLIC "${SODIUM_DIR}/src/libsodium/include")
endif()

# meshoptimizer - for mesh optimization
set(MESHOPTIMIZER_DIR "${EXTERNAL_DIR}/meshoptimizer")
if(EXISTS "${MESHOPTIMIZER_DIR}")
    message(STATUS "meshoptimizer found at ${MESHOPTIMIZER_DIR}")
    target_include_directories(poko_core PUBLIC "${MESHOPTIMIZER_DIR}/src")
endif()

# zstd - for compression
set(ZSTD_DIR "${EXTERNAL_DIR}/zstd")
if(EXISTS "${ZSTD_DIR}")
    message(STATUS "zstd found at ${ZSTD_DIR}")
    target_include_directories(poko_core PUBLIC "${ZSTD_DIR}/lib")
endif()

# Mute Language Integration
set(MUTE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../mute")
if(EXISTS "${MUTE_DIR}")
    message(STATUS "Mute language found at ${MUTE_DIR}")
    
    # Add Mute as a subdirectory
    add_subdirectory(${MUTE_DIR} ${CMAKE_BINARY_DIR}/mute EXCLUDE_FROM_ALL)
    
    # Link Mute library to engine
    target_link_libraries(poko_core PUBLIC mute)
    
    target_include_directories(poko_core PUBLIC
        $<BUILD_INTERFACE:${MUTE_DIR}/include>
    )
    
    target_compile_definitions(poko_core PUBLIC MUTE_INTEGRATION_ENABLED)
else()
    message(WARNING "Mute language not found - scripting will be disabled")
endif()

# Link Diligent Engine if available (temporarily disabled)
# if(EXISTS "${DILIGENT_DIR}" AND EXISTS "${DILIGENT_CORE_DIR}")
#     target_link_libraries(poko_core PUBLIC DiligentCore DiligentEngine)
# endif()

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

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
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
