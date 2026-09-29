cmake_minimum_required(VERSION 3.14)
project(PokoEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core memory library
add_library(poko_core_memory
    # Allocator interface
    src/core/memory/allocators/interface.cpp
    
    # System allocator
    src/core/memory/allocators/system/constructor.cpp
    src/core/memory/allocators/system/allocate.cpp
    src/core/memory/allocators/system/deallocate.cpp
    src/core/memory/allocators/system/stats.cpp
    src/core/memory/allocators/system/reset.cpp
    src/core/memory/allocators/system/getter.cpp
    src/core/memory/allocators/system/info.cpp
    src/core/memory/allocators/system/tracking.cpp
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

# Platform-specific settings
if(WIN32)
    target_compile_definitions(poko_core_memory PRIVATE _WIN32)
    target_compile_definitions(poko_core_handles PRIVATE _WIN32)
    target_compile_definitions(poko_core_time PRIVATE _WIN32)
    target_compile_definitions(poko_core_events PRIVATE _WIN32)
    target_compile_definitions(poko_core_jobs PRIVATE _WIN32)
    target_compile_definitions(poko_core_configuration PRIVATE _WIN32)
endif()

# Production build flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(poko_core_memory PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_handles PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_time PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_events PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_jobs PRIVATE -O3 -DNDEBUG)
    target_compile_options(poko_core_configuration PRIVATE -O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(poko_core_memory PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_handles PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_time PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_events PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_jobs PRIVATE -g -O0 -DDEBUG)
    target_compile_options(poko_core_configuration PRIVATE -g -O0 -DDEBUG)
endif()

# Enable testing
enable_testing()

# Unit tests
add_executable(test_allocator
    tests/unit/core/memory/test_allocator.cpp
)

target_link_libraries(test_allocator PRIVATE poko_core_memory)
add_test(NAME test_allocator COMMAND test_allocator)

add_executable(test_handle
    tests/unit/core/handles/test_handle.cpp
)

target_link_libraries(test_handle PRIVATE poko_core_handles)
add_test(NAME test_handle COMMAND test_handle)

add_executable(test_time
    tests/unit/core/time/test_time.cpp
)

target_link_libraries(test_time PRIVATE poko_core_time)
add_test(NAME test_time COMMAND test_time)

add_executable(test_event
    tests/unit/core/events/test_event.cpp
)

target_link_libraries(test_event PRIVATE poko_core_events)
add_test(NAME test_event COMMAND test_event)

add_executable(test_job
    tests/unit/core/jobs/test_job.cpp
)

target_link_libraries(test_job PRIVATE poko_core_jobs)
add_test(NAME test_job COMMAND test_job)

add_executable(test_configuration
    tests/unit/core/configuration/test_configuration.cpp
)

target_link_libraries(test_configuration PRIVATE poko_core_configuration)
add_test(NAME test_configuration COMMAND test_configuration)

# Installation
install(TARGETS poko_core_memory poko_core_handles poko_core_time poko_core_events poko_core_jobs poko_core_configuration
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/poko_engine)