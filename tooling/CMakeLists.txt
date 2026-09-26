cmake_minimum_required(VERSION 3.20)
project(PokoTooling VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tooling build options
option(TOOLING_BUILD_TESTS "Build tooling tests" ON)

# Tooling directories
set(TOOLING_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TOOLING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files - PNV system
file(GLOB_RECURSE TOOLING_PNV_SOURCES
    "${TOOLING_SOURCE_DIR}/pnv/*.cpp"
    "${TOOLING_SOURCE_DIR}/pnv/*.cc"
)

file(GLOB_RECURSE TOOLING_PNV_HEADERS
    "${TOOLING_INCLUDE_DIR}/pnv/*.h"
    "${TOOLING_INCLUDE_DIR}/pnv/*.hpp"
)

# PNV library
if(TOOLING_PNV_SOURCES OR TOOLING_PNV_HEADERS)
    add_library(poko_pnv STATIC
        ${TOOLING_PNV_SOURCES}
        ${TOOLING_PNV_HEADERS}
    )

    target_include_directories(poko_pnv PUBLIC
        $<BUILD_INTERFACE:${TOOLING_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    target_compile_features(poko_pnv PUBLIC cxx_std_20)

    # Compiler warnings
    if(MSVC)
        target_compile_options(poko_pnv PRIVATE /W4)
    else()
        target_compile_options(poko_pnv PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Installation
    install(TARGETS poko_pnv
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )

    message(STATUS "Poko PNV system configured")
else()
    message(STATUS "Poko PNV sources not yet implemented")
endif()

# Placeholder for other tooling components
# Will be implemented as part of foundation components:
# - Build pipelines
# - Testing framework
# - Diagnostics
# - Formatting
