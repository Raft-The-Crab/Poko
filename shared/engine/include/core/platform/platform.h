/**
 * @file platform.h
 * @brief Platform layer for cross-platform abstractions
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_PLATFORM_PLATFORM_H
#define POKO_CORE_PLATFORM_PLATFORM_H

#include <cstdint>
#include <string>
#include <vector>

namespace poko {
namespace core {
namespace platform {

// ============================================================================
// Platform Detection
// ============================================================================

/// Platform types
enum class Platform : uint32_t {
    Unknown = 0,
    Windows = 1,
    Linux = 2,
    macOS = 3,
    Android = 4,
    iOS = 5,
    Web = 6
};

/// Architecture types
enum class Architecture : uint32_t {
    Unknown = 0,
    x86 = 1,
    x86_64 = 2,
    ARM = 3,
    ARM64 = 4
};

/// Endianness
enum class Endianness : uint32_t {
    Unknown = 0,
    Little = 1,
    Big = 2
};

// ============================================================================
// Platform Information
// ============================================================================

/**
 * @brief Get current platform
 * @returns Current platform type
 */
Platform getPlatform() noexcept;

/**
 * @brief Get platform name as string
 * @returns Platform name string
 */
const char* getPlatformName() noexcept;

/**
 * @brief Get current architecture
 * @returns Current architecture type
 */
Architecture getArchitecture() noexcept;

/**
 * @brief Get architecture name as string
 * @returns Architecture name string
 */
const char* getArchitectureName() noexcept;

/**
 * @brief Get system endianness
 * @returns System endianness
 */
Endianness getEndianness() noexcept;

/**
 * @brief Check if system is little-endian
 * @returns true if little-endian, false otherwise
 */
bool isLittleEndian() noexcept;

/**
 * @brief Check if system is big-endian
 * @returns true if big-endian, false otherwise
 */
bool isBigEndian() noexcept;

// ============================================================================
// CPU Information
// ============================================================================

/**
 * @brief CPU information structure
 */
struct CPUInfo {
    uint32_t coreCount;           ///< Number of physical CPU cores
    uint32_t logicalCoreCount;    ///< Number of logical CPU cores (with hyperthreading)
    uint64_t frequencyHz;         ///< CPU frequency in Hz
    std::string vendor;           ///< CPU vendor string (e.g., "GenuineIntel")
    std::string model;            ///< CPU model string
    bool hasSSE;                  ///< SSE support
    bool hasSSE2;                 ///< SSE2 support
    bool hasAVX;                  ///< AVX support
    bool hasAVX2;                 ///< AVX2 support
    bool hasAVX512;               ///< AVX-512 support
    bool hasNEON;                 ///< NEON support (ARM)
};

/**
 * @brief Get CPU information
 * @returns CPU information structure
 */
CPUInfo getCPUInfo() noexcept;

/**
 * @brief Get number of CPU cores
 * @returns Number of physical CPU cores
 */
uint32_t getCoreCount() noexcept;

/**
 * @brief Get number of logical CPU cores
 * @returns Number of logical CPU cores (with hyperthreading)
 */
uint32_t getLogicalCoreCount() noexcept;

// ============================================================================
// Memory Information
// ============================================================================

/**
 * @brief Memory information structure
 */
struct MemoryInfo {
    uint64_t totalPhysicalMB;     ///< Total physical memory in MB
    uint64_t availablePhysicalMB;  ///< Available physical memory in MB
    uint64_t totalVirtualMB;       ///< Total virtual memory in MB
    uint64_t availableVirtualMB;   ///< Available virtual memory in MB
    uint64_t pageSize;             ///< Memory page size in bytes
};

/**
 * @brief Get memory information
 * @returns Memory information structure
 */
MemoryInfo getMemoryInfo() noexcept;

/**
 * @brief Get total physical memory in MB
 * @returns Total physical memory in MB
 */
uint64_t getTotalPhysicalMemoryMB() noexcept;

/**
 * @brief Get available physical memory in MB
 * @returns Available physical memory in MB
 */
uint64_t getAvailablePhysicalMemoryMB() noexcept;

/**
 * @brief Get memory page size
 * @returns Memory page size in bytes
 */
uint64_t getPageSize() noexcept;

// ============================================================================
// System Information
// ============================================================================

/**
 * @brief System information structure
 */
struct SystemInfo {
    std::string osName;            ///< Operating system name
    std::string osVersion;         ///< Operating system version
    std::string hostName;          ///< Host name
    std::string userName;          ///< Current user name
    std::string architecture;      ///< Architecture string
    Platform platform;             ///< Platform type
    Architecture arch;             ///< Architecture type
};

/**
 * @brief Get system information
 * @returns System information structure
 */
SystemInfo getSystemInfo() noexcept;

/**
 * @brief Get OS name
 * @returns Operating system name string
 */
std::string getOSName();

/**
 * @brief Get OS version
 * @returns Operating system version string
 */
std::string getOSVersion();

/**
 * @brief Get host name
 * @returns Host name string
 */
std::string getHostName();

/**
 * @brief Get user name
 * @returns Current user name string
 */
std::string getUserName();

// ============================================================================
// Process Information
// ============================================================================

/**
 * @brief Process information structure
 */
struct ProcessInfo {
    uint64_t processId;           ///< Process ID
    uint64_t parentId;            ///< Parent process ID
    uint64_t memoryUsageMB;       ///< Memory usage in MB
    uint32_t threadCount;         ///< Number of threads
    std::string executablePath;   ///< Executable path
    std::string workingDirectory; ///< Working directory
};

/**
 * @brief Get current process information
 * @returns Process information structure
 */
ProcessInfo getProcessInfo() noexcept;

/**
 * @brief Get current process ID
 * @returns Current process ID
 */
uint64_t getCurrentProcessId() noexcept;

/**
 * @brief Get current process memory usage in MB
 * @returns Memory usage in MB
 */
uint64_t getProcessMemoryUsageMB() noexcept;

// ============================================================================
// Thread Utilities
// ============================================================================

/**
 * @brief Get current thread ID
 * @returns Current thread ID
 */
uint64_t getCurrentThreadId() noexcept;

/**
 * @brief Set thread name (platform-specific)
 * @param name Thread name
 * @returns true if successful, false otherwise
 */
bool setThreadName(const std::string& name);

/**
 * @brief Get thread name (platform-specific)
 * @returns Thread name string
 */
std::string getThreadName();

/**
 * @brief Set thread affinity (bind to specific cores)
 * @param coreIndices Vector of core indices to bind to
 * @returns true if successful, false otherwise
 */
bool setThreadAffinity(const std::vector<uint32_t>& coreIndices);

// ============================================================================
// Environment Variables
// ============================================================================

/**
 * @brief Get environment variable
 * @param name Environment variable name
 * @returns Environment variable value, or empty string if not found
 */
std::string getEnv(const std::string& name);

/**
 * @brief Set environment variable
 * @param name Environment variable name
 * @param value Environment variable value
 * @returns true if successful, false otherwise
 */
bool setEnv(const std::string& name, const std::string& value);

/**
 * @brief Unset environment variable
 * @param name Environment variable name
 * @returns true if successful, false otherwise
 */
bool unsetEnv(const std::string& name);

// ============================================================================
// Constants
// ============================================================================

/// Maximum string length for platform strings
constexpr size_t MAX_PLATFORM_STRING_LENGTH = 256;

/// Maximum environment variable name length
constexpr size_t MAX_ENV_NAME_LENGTH = 128;

/// Maximum environment variable value length
constexpr size_t MAX_ENV_VALUE_LENGTH = 4096;

} // namespace platform
} // namespace core
} // namespace poko

#endif // POKO_CORE_PLATFORM_PLATFORM_H
