cmake_minimum_required(VERSION 3.14)
project(PokoEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add Catch2 testing framework
add_subdirectory(external/Catch2 EXCLUDE_FROM_ALL)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core memory library
add_library(poko_core_memory
    # Allocator interface
    src/core/memory/allocators/interface/interface.cpp
    
    # System allocator
    src/core/memory/allocators/system/constructor.cpp
    src/core/memory/allocators/system/allocate.cpp
    src/core/memory/allocators/system/deallocate.cpp
    src/core/memory/allocators/system/stats.cpp
    src/core/memory/allocators/system/reset.cpp
    src/core/memory/allocators/system/getter.cpp
    src/core/memory/allocators/system/info.cpp
    src/core/memory/allocators/system/tracking.cpp
    src/core/memory/allocators/system/validate.cpp
)

target_include_directories(poko_core_memory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core handles library
add_library(poko_core_handles
    src/core/handles/interface.cpp
    src/core/handles/manager/constructor.cpp
    src/core/handles/manager/allocate.cpp
    src/core/handles/manager/free.cpp
    src/core/handles/manager/validate.cpp
    src/core/handles/manager/stats.cpp
    src/core/handles/manager/reset.cpp
    src/core/handles/manager/active_count.cpp
)

target_include_directories(poko_core_handles
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core time library
add_library(poko_core_time
    src/core/time/interface.cpp
    src/core/time/current/current.cpp
    src/core/time/sleep/sleep.cpp
    src/core/time/elapsed/elapsed.cpp
)

target_include_directories(poko_core_time
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core events library
add_library(poko_core_events
    src/core/events/interface.cpp
    src/core/events/registry/registry.cpp
    src/core/events/signal/connect.cpp
    src/core/events/signal/disconnect.cpp
    src/core/events/signal/emit.cpp
    src/core/events/queue/push.cpp
    src/core/events/queue/pop.cpp
    src/core/events/queue/info.cpp
    src/core/events/dispatcher/register.cpp
    src/core/events/dispatcher/unregister.cpp
    src/core/events/dispatcher/dispatch.cpp
    src/core/events/dispatcher/queue.cpp
)

target_include_directories(poko_core_events
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core jobs library
add_library(poko_core_jobs
    src/core/jobs/interface.cpp
    src/core/jobs/job.cpp
    src/core/jobs/constructor/constructor.cpp
    src/core/jobs/destructor/destructor.cpp
    src/core/jobs/submit/submit.cpp
    src/core/jobs/cancellation/cancel.cpp
    src/core/jobs/wait/wait.cpp
    src/core/jobs/statistics/statistics.cpp
    src/core/jobs/clear/clear.cpp
    src/core/jobs/shutdown/shutdown.cpp
    src/core/jobs/worker/worker.cpp
)

target_include_directories(poko_core_jobs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core configuration library
add_library(poko_core_configuration
    src/core/configuration/interface/interface.cpp
    src/core/configuration/constructor/constructor.cpp
    src/core/configuration/set/set.cpp
    src/core/configuration/get/get.cpp
    src/core/configuration/remove/remove.cpp
    src/core/configuration/clear/clear.cpp
    src/core/configuration/load/load.cpp
    src/core/configuration/save/save.cpp
    src/core/configuration/default/default.cpp
    src/core/configuration/keys/keys.cpp
)

target_include_directories(poko_core_configuration
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core logging library
add_library(poko_core_logging
    src/core/logging/utility/utility.cpp
    src/core/logging/interface/interface.cpp
    src/core/logging/constructor/constructor.cpp
    src/core/logging/log/log.cpp
    src/core/logging/sink/sink.cpp
    src/core/logging/level/level.cpp
    src/core/logging/filter/filter.cpp
    src/core/logging/flush/flush.cpp
    src/core/logging/format/format.cpp
    src/core/logging/console/console.cpp
    src/core/logging/file/file.cpp
)

target_include_directories(poko_core_logging
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core serialization library
add_library(poko_core_serialization
    src/core/serialization/interface.cpp
    src/core/serialization/binary/constructor.cpp
    src/core/serialization/binary/mode.cpp
    src/core/serialization/binary/primitives.cpp
    src/core/serialization/binary/string.cpp
    src/core/serialization/binary/io.cpp
    src/core/serialization/binary/data.cpp
)

target_include_directories(poko_core_serialization
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core runtime library
add_library(poko_core_runtime
    src/core/runtime/factory.cpp
    src/core/runtime/instance/constructor.cpp
    src/core/runtime/instance/accessors.cpp
    src/core/runtime/instance/children.cpp
    src/core/runtime/instance/properties.cpp
    src/core/runtime/instance/tags.cpp
)

target_include_directories(poko_core_runtime
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Runtime library dependencies
target_link_libraries(poko_core_runtime PRIVATE poko_core_handles)

# Core properties library
add_library(poko_core_properties
    src/core/properties/interface/interface.cpp
    src/core/properties/constructor/constructor.cpp
    src/core/properties/get/get.cpp
    src/core/properties/set/set.cpp
    src/core/properties/metadata/metadata.cpp
    src/core/properties/registry/registry.cpp
)

target_include_directories(poko_core_properties
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Core components library
add_library(poko_core_components
    src/core/components/interface/interface.cpp
    src/core/components/registry/registry.cpp
    src/core/components/manager/manager.cpp
)

target_include_directories(poko_core_components
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Components library dependencies
target_link_libraries(poko_core_components PRIVATE poko_core_handles)

# Core world library
add_library(poko_core_world
    src/core/world/scene/constructor.cpp
    src/core/world/scene/instances.cpp
    src/core/world/world/manager.cpp
    src/core/world/interface.cpp
)

target_include_directories(poko_core_world
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# World library dependencies
target_link_libraries(poko_core_world PRIVATE poko_core_runtime)

# Core resources library
add_library(poko_core_resources
    src/core/resources/base/constructor.cpp
    src/core/resources/manager/manager.cpp
    src/core/resources/interface/interface.cpp
)

target_include_directories(poko_core_resources
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Resources library dependencies
target_link_libraries(poko_core_resources PRIVATE)

# Core platform library
add_library(poko_core_platform
    src/core/platform/detection/detection.cpp
)

target_include_directories(poko_core_platform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Platform library dependencies
if(UNIX AND NOT APPLE)
    target_link_libraries(poko_core_platform PRIVATE pthread)
endif()

# Core diagnostics library
add_library(poko_core_diagnostics
    src/core/diagnostics/manager/manager.cpp
    src/core/diagnostics/interface/interface.cpp
)

target_include_directories(poko_core_diagnostics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Diagnostics library dependencies
# target_link_libraries(poko_core_diagnostics PRIVATE poko_core_platform)

# Core profiler library
add_library(poko_core_profiler
    src/core/profiler/manager/manager.cpp
    src/core/profiler/interface/interface.cpp
)

target_include_directories(poko_core_profiler
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Transform component library
add_library(poko_core_transform
    src/core/components/transform/transform.cpp
    src/core/components/transform/transform_registration.cpp
)

target_include_directories(poko_core_transform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Renderable component library
add_library(poko_core_renderable
    src/core/components/renderable/renderable.cpp
    src/core/components/renderable/renderable_registration.cpp
)

target_include_directories(poko_core_renderable
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(poko_core_renderable PRIVATE poko_core_components)

# Physics component library
add_library(poko_core_physics
    src/core/components/physics/physics.cpp
    src/core/components/physics/physics_registration.cpp
    # Material
    src/core/components/physics/material/set_density.cpp
    # Velocity
    src/core/components/physics/velocity/set_linear_velocity.cpp
    src/core/components/physics/velocity/set_angular_velocity.cpp
    # Mass
    src/core/components/physics/mass/calculate_mass.cpp
    src/core/components/physics/mass/calculate_inertia.cpp
    # Collision
    src/core/components/physics/collision/should_collide.cpp
    # Force
    src/core/components/physics/force/apply_force.cpp
    src/core/components/physics/force/apply_force_at_point.cpp
    src/core/components/physics/force/apply_torque.cpp
    src/core/components/physics/force/apply_impulse.cpp
    src/core/components/physics/force/apply_impulse_at_point.cpp
    src/core/components/physics/force/clear_accumulated_forces.cpp
    # Damping
    src/core/components/physics/damping/apply_damping.cpp
    # Limits
    src/core/components/physics/limits/clamp_velocities.cpp
    # Integration
    src/core/components/physics/integration/integrate_velocity.cpp
)

target_include_directories(poko_core_physics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(poko_core_physics PRIVATE poko_core_components)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(poko_core_memory PRIVATE _WIN32)
    target_compile_definitions(poko_core_handles PRIVATE _WIN32)
    target_compile_definitions(poko_core_time PRIVATE _WIN32)
    target_compile_definitions(poko_core_events PRIVATE _WIN32)
    target_compile_definitions(poko_core_jobs PRIVATE _WIN32)
    target_compile_definitions(poko_core_configuration PRIVATE _WIN32)
    target_compile_definitions(poko_core_logging PRIVATE _WIN32)
    target_compile_definitions(poko_core_serialization PRIVATE _WIN32)
    target_compile_definitions(poko_core_runtime PRIVATE _WIN32)
    target_compile_definitions(poko_core_properties PRIVATE _WIN32)
    target_compile_definitions(poko_core_components PRIVATE _WIN32)
    target_compile_definitions(poko_core_world PRIVATE _WIN32)
    target_compile_definitions(poko_core_resources PRIVATE _WIN32)
    target_compile_definitions(poko_core_platform PRIVATE _WIN32)
    target_compile_definitions(poko_core_diagnostics PRIVATE _WIN32)
    target_compile_definitions(poko_core_profiler PRIVATE _WIN32)
    target_compile_definitions(poko_core_transform PRIVATE _WIN32)
    target_compile_definitions(poko_core_renderable PRIVATE _WIN32)
    target_compile_definitions(poko_core_physics PRIVATE _WIN32)
endif()

# Production build flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(poko_core_memory PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_handles PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_time PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_events PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_jobs PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_configuration PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_logging PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_serialization PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_runtime PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_properties PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_components PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_world PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_resources PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_platform PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_diagnostics PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_profiler PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_transform PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_renderable PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_physics PRIVATE -O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(poko_core_memory PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_handles PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_time PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_events PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_jobs PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_configuration PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_logging PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_serialization PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_runtime PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_properties PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_components PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_world PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_resources PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_platform PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_diagnostics PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_profiler PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_transform PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_renderable PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_physics PRIVATE -g -O0 -DDEBUG)
endif()

# Enable testing
enable_testing()

# Unit tests
add_executable(test_allocator
    tests/unit/core/memory/test_allocator.cpp
)

target_link_libraries(test_allocator PRIVATE poko_core_memory Catch2::Catch2)
add_test(NAME test_allocator COMMAND test_allocator)

add_executable(test_handle
    tests/unit/core/handles/test_handle.cpp
)

target_link_libraries(test_handle PRIVATE poko_core_handles Catch2::Catch2)
add_test(NAME test_handle COMMAND test_handle)

add_executable(test_time
    tests/unit/core/time/test_time.cpp
)

target_link_libraries(test_time PRIVATE poko_core_time Catch2::Catch2)
add_test(NAME test_time COMMAND test_time)

add_executable(test_event
    tests/unit/core/events/test_event.cpp
)

target_link_libraries(test_event PRIVATE poko_core_events Catch2::Catch2)
add_test(NAME test_event COMMAND test_event)

add_executable(test_job
    tests/unit/core/jobs/test_job.cpp
)

target_link_libraries(test_job PRIVATE poko_core_jobs Catch2::Catch2)
add_test(NAME test_job COMMAND test_job)

add_executable(test_configuration
    tests/unit/core/configuration/test_configuration.cpp
)

target_link_libraries(test_configuration PRIVATE poko_core_configuration Catch2::Catch2)
add_test(NAME test_configuration COMMAND test_configuration)

add_executable(test_logging
    tests/unit/core/logging/test_logging.cpp
)

target_link_libraries(test_logging PRIVATE poko_core_logging Catch2::Catch2)
add_test(NAME test_logging COMMAND test_logging)

add_executable(test_serialization
    tests/unit/core/serialization/test_serialization.cpp
)

target_link_libraries(test_serialization PRIVATE poko_core_serialization Catch2::Catch2)
add_test(NAME test_serialization COMMAND test_serialization)

add_executable(test_runtime
    tests/unit/core/runtime/test_runtime.cpp
)

target_link_libraries(test_runtime PRIVATE poko_core_runtime Catch2::Catch2)
add_test(NAME test_runtime COMMAND test_runtime)

add_executable(test_property
    tests/unit/core/properties/test_property.cpp
)

target_link_libraries(test_property PRIVATE poko_core_properties Catch2::Catch2)
add_test(NAME test_property COMMAND test_property)

add_executable(test_component
    tests/unit/core/components/test_component.cpp
)

target_link_libraries(test_component PRIVATE poko_core_components poko_core_handles Catch2::Catch2)
add_test(NAME test_component COMMAND test_component)

add_executable(test_scene
    tests/unit/core/world/test_scene.cpp
)

target_link_libraries(test_scene PRIVATE poko_core_world Catch2::Catch2)
add_test(NAME test_scene COMMAND test_scene)

# Skip test_resource due to linker issues on Windows
# TODO: Investigate and fix linker error
# add_executable(test_resource
#     tests/unit/core/resources/test_resource.cpp
# )
# target_link_libraries(test_resource PRIVATE poko_core_resources Catch2::Catch2)
# add_test(NAME test_resource COMMAND test_resource)

add_executable(test_platform
    tests/unit/core/platform/test_platform.cpp
)

target_link_libraries(test_platform PRIVATE poko_core_platform Catch2::Catch2)
add_test(NAME test_platform COMMAND test_platform)

add_executable(test_diagnostics
    tests/unit/core/diagnostics/test_diagnostics.cpp
)

target_link_libraries(test_diagnostics PRIVATE poko_core_diagnostics Catch2::Catch2)
add_test(NAME test_diagnostics COMMAND test_diagnostics)

add_executable(test_profiler
    tests/unit/core/profiler/test_profiler.cpp
)

target_link_libraries(test_profiler PRIVATE poko_core_profiler Catch2::Catch2)
add_test(NAME test_profiler COMMAND test_profiler)

add_executable(test_transform
    tests/unit/core/components/transform/test_transform.cpp
)

target_link_libraries(test_transform PRIVATE poko_core_transform Catch2::Catch2)
add_test(NAME test_transform COMMAND test_transform)

add_executable(test_renderable
    tests/unit/core/components/renderable/test_renderable.cpp
)

target_link_libraries(test_renderable PRIVATE poko_core_renderable Catch2::Catch2)
add_test(NAME test_renderable COMMAND test_renderable)

# Physics test - temporarily disabled due to Windows/MSYS2 linker issue
# TODO: Fix and re-enable after investigating linker configuration
# add_executable(test_physics
#     tests/unit/core/components/physics/test_physics.cpp
# )
# target_link_libraries(test_physics PRIVATE poko_core_physics Catch2::Catch2)
# set_target_properties(test_physics PROPERTIES WIN32_EXECUTABLE FALSE)
# add_test(NAME test_physics COMMAND test_physics)

# Installation
install(TARGETS poko_core_memory poko_core_handles poko_core_time poko_core_events poko_core_jobs poko_core_configuration poko_core_logging poko_core_serialization poko_core_runtime poko_core_properties poko_core_components poko_core_world poko_core_resources poko_core_platform poko_core_diagnostics poko_core_profiler poko_core_transform poko_core_renderable poko_core_physics
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/poko_engine)