cmake_minimum_required(VERSION 3.10)
project(PokoEngine VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    # add_compile_options(-Werror) # Disable warnings as errors for now
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../mute/include
    ${CMAKE_SOURCE_DIR}/../mute/build/include
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/core/ecs
    ${CMAKE_SOURCE_DIR}/src/core/jobs
    ${CMAKE_SOURCE_DIR}/src/core/window
    ${CMAKE_SOURCE_DIR}/src/core/logging
    ${CMAKE_SOURCE_DIR}/src/core/profiling
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/rendering/bgfx
    ${CMAKE_SOURCE_DIR}/src/rendering/pipelines
    ${CMAKE_SOURCE_DIR}/src/scripting
    ${CMAKE_SOURCE_DIR}/src/physics
    ${CMAKE_SOURCE_DIR}/src/physics/jolt
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/src/audio/mixer
    ${CMAKE_SOURCE_DIR}/src/networking
    ${CMAKE_SOURCE_DIR}/src/networking/websocket
    ${CMAKE_SOURCE_DIR}/src/networking/bitbuffer
)

# Find dependencies
find_package(Threads REQUIRED)

# Use pkg-config for MSYS2 dependencies
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(OPENAL openal)
    pkg_check_modules(LIBWEBSOCKETS libwebsockets)
    pkg_check_modules(JOLT joltphysics)

    include_directories(${SDL2_INCLUDE_DIRS})
    if(OPENAL_FOUND)
        include_directories(${OPENAL_INCLUDE_DIRS})
    endif()
    if(LIBWEBSOCKETS_FOUND)
        include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
    endif()
    if(JOLT_FOUND)
        include_directories(${JOLT_INCLUDE_DIRS})
    endif()

    # GLM is header-only, just include the directory
    include_directories(/ucrt64/include)
else()
    # Fallback to manual MSYS2 paths
    message(WARNING "pkg-config not found, using manual MSYS2 paths")
    include_directories(/ucrt64/include/SDL2)
    include_directories(/ucrt64/include)
    include_directories(/ucrt64/include/AL)
    include_directories(/ucrt64/include/libwebsockets)
    include_directories(/ucrt64/include/Jolt/Jolt)
    # Comment out bgfx includes until it's properly installed
    # include_directories(/ucrt64/include/bgfx)
    # include_directories(/ucrt64/include/bx)
    # include_directories(/ucrt64/include/bimg)

    # Link directories
    link_directories(/ucrt64/lib)
    link_directories(${CMAKE_SOURCE_DIR}/../mute/build)
endif()

# Core engine library
add_library(poko_core STATIC
    src/core/engine/engine.cpp
    src/core/config/config.cpp
    src/core/ecs/entity.cpp
    src/core/ecs/component_manager.cpp
    src/core/jobs/job.cpp
    src/core/jobs/job_manager.cpp
    src/core/window/window.cpp
    src/core/logging/logger.cpp
    src/core/profiling/profiler.cpp
    src/input/input.cpp
    src/rendering/bgfx/bgfx_renderer.cpp
    src/rendering/pipelines/render_pipeline.cpp
    # src/physics/jolt/physics_world.cpp  # Only include if Jolt is available
    src/audio/mixer/audio_mixer.cpp
    src/networking/websocket/websocket_client.cpp
    # src/scripting/mute_engine.cpp
    # src/scripting/engine_bindings.cpp
)

target_link_libraries(poko_core PRIVATE Threads::Threads)

# Only include physics if Jolt is available
if(PKG_CONFIG_FOUND AND JOLT_FOUND)
    target_sources(poko_core PRIVATE src/physics/jolt/physics_world.cpp)
    target_link_libraries(poko_core PRIVATE ${JOLT_LIBRARIES})
elseif(EXISTS "${CMAKE_SOURCE_DIR}/src/physics/jolt/physics_world.cpp")
    # Try to check if Jolt library exists
    find_library(JOLT_LIB NAMES Jolt Jolt.lib PATHS /ucrt64/lib)
    if(JOLT_LIB)
        target_sources(poko_core PRIVATE src/physics/jolt/physics_world.cpp)
        target_link_libraries(poko_core PRIVATE Jolt Jolt.lib -lJolt Jolt.dll.a)
    else()
        message(WARNING "Jolt library not found, excluding physics from core library")
    endif()
endif()

# Add SDL2 linking if available
if(PKG_CONFIG_FOUND)
    target_link_libraries(poko_core PRIVATE ${SDL2_LIBRARIES})
    if(OPENAL_FOUND)
        target_link_libraries(poko_core PRIVATE ${OPENAL_LIBRARIES})
    endif()
    if(LIBWEBSOCKETS_FOUND)
        target_link_libraries(poko_core PRIVATE ${LIBWEBSOCKETS_LIBRARIES})
    endif()
    if(JOLT_FOUND)
        target_link_libraries(poko_core PRIVATE ${JOLT_LIBRARIES})
    endif()
    if(GLM_FOUND)
        target_link_libraries(poko_core PRIVATE ${GLM_LIBRARIES})
    endif()
else()
    target_link_libraries(poko_core PRIVATE SDL2main SDL2)
    target_link_libraries(poko_core PRIVATE OpenAL32)
    target_link_libraries(poko_core PRIVATE websockets)
    # Only link Jolt if physics was included
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/physics/jolt/physics_world.cpp")
        find_library(JOLT_LIB NAMES Jolt Jolt.lib PATHS /ucrt64/lib)
        if(JOLT_LIB)
            target_link_libraries(poko_core PRIVATE Jolt Jolt.lib -lJolt Jolt.dll.a)
        endif()
    endif()
    target_link_libraries(poko_core PRIVATE glm)
    # Comment out bgfx until it's properly installed
    # target_link_libraries(poko_core PRIVATE bgfx bx bimg)
endif()

# Main engine library (links all subsystems)
# For now, skip the interface library and use poko_core directly
# add_library(poko_engine INTERFACE)
# target_link_libraries(poko_engine INTERFACE poko_core Threads::Threads)

# Add dependency libraries based on pkg-config availability
# For now, skip this since we're not using SDL yet
# if(PKG_CONFIG_FOUND)
#     target_link_libraries(poko_engine PRIVATE ${SDL2_LIBRARIES})
# else()
#     target_link_libraries(poko_engine PRIVATE SDL2main SDL2)
# endif()

# Test executables
add_executable(test_engine
    tests/core/test_engine.cpp
)

target_link_libraries(test_engine
    PRIVATE
    poko_core
    Threads::Threads
)

add_executable(test_config
    tests/core/test_config.cpp
)

target_link_libraries(test_config
    PRIVATE
    poko_core
)

add_executable(test_ecs
    tests/core/test_ecs.cpp
)

target_link_libraries(test_ecs
    PRIVATE
    poko_core
)

add_executable(test_ecs_queries
    tests/core/test_ecs_queries.cpp
)

target_link_libraries(test_ecs_queries
    PRIVATE
    poko_core
)

add_executable(test_jobs
    tests/core/test_jobs.cpp
)

target_link_libraries(test_jobs
    PRIVATE
    poko_core
    Threads::Threads
)

add_executable(test_window
    tests/core/test_window.cpp
)

target_link_libraries(test_window
    PRIVATE
    poko_core
)

add_executable(test_window_events
    tests/core/test_window_events.cpp
)

target_link_libraries(test_window_events
    PRIVATE
    poko_core
)

# Physics tests (may fail if Jolt library isn't properly installed)
# Only build these if Jolt linking succeeds
if(DEFINED JOLT_FOUND AND JOLT_FOUND)
    add_executable(test_physics
        tests/core/test_physics.cpp
    )

    target_link_libraries(test_physics
        PRIVATE
        poko_core
    )

    # Add Jolt linking for test_physics as well
    if(PKG_CONFIG_FOUND)
        if(JOLT_FOUND)
            target_link_libraries(test_physics PRIVATE ${JOLT_LIBRARIES})
        endif()
    else()
        target_link_libraries(test_physics PRIVATE Jolt Jolt.lib -lJolt Jolt.dll.a)
    endif()

    add_executable(test_physics_advanced
        tests/core/test_physics_advanced.cpp
    )

    target_link_libraries(test_physics_advanced
        PRIVATE
        poko_core
    )

    # Add Jolt linking for test_physics_advanced as well
    if(PKG_CONFIG_FOUND)
        if(JOLT_FOUND)
            target_link_libraries(test_physics_advanced PRIVATE ${JOLT_LIBRARIES})
        endif()
    else()
        target_link_libraries(test_physics_advanced PRIVATE Jolt Jolt.lib -lJolt Jolt.dll.a)
    endif()
else()
    message(WARNING "Jolt physics library not found, skipping physics tests")
endif()

add_executable(test_audio
    tests/core/test_audio.cpp
)

target_link_libraries(test_audio
    PRIVATE
    poko_core
)

add_executable(test_websocket_advanced
    tests/core/test_websocket_advanced.cpp
)

target_link_libraries(test_websocket_advanced
    PRIVATE
    poko_core
)

add_executable(test_renderer
    tests/core/test_renderer.cpp
)

target_link_libraries(test_renderer
    PRIVATE
    poko_core
)

# Installation
install(TARGETS poko_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)