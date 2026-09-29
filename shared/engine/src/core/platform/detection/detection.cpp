/**
 * @file detection.cpp
 * @brief Platform detection implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/platform/platform.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <psapi.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <sys/sysinfo.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <sys/utsname.h>
    #include <sys/syscall.h>
#elif defined(__APPLE__)
    #include <unistd.h>
    #include <sys/sysctl.h>
    #include <mach/mach.h>
    #include <mach/mach_host.h>
    #include <pwd.h>
    #include <pthread.h>
    #include <TargetConditionals.h>
#endif

#include <cstring>

namespace poko {
namespace core {
namespace platform {

Platform getPlatform() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    return Platform::Windows;
#elif defined(__linux__)
    return Platform::Linux;
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        return Platform::iOS;
    #else
        return Platform::macOS;
    #endif
#elif defined(__ANDROID__)
    return Platform::Android;
#elif defined(__EMSCRIPTEN__)
    return Platform::Web;
#else
    return Platform::Unknown;
#endif
}

const char* getPlatformName() noexcept {
    Platform platform = getPlatform();
    switch (platform) {
        case Platform::Windows: return "Windows";
        case Platform::Linux: return "Linux";
        case Platform::macOS: return "macOS";
        case Platform::Android: return "Android";
        case Platform::iOS: return "iOS";
        case Platform::Web: return "Web";
        default: return "Unknown";
    }
}

Architecture getArchitecture() noexcept {
#if defined(_M_X64) || defined(__x86_64__)
    return Architecture::x86_64;
#elif defined(_M_IX86) || defined(__i386__)
    return Architecture::x86;
#elif defined(_M_ARM64) || defined(__aarch64__)
    return Architecture::ARM64;
#elif defined(_M_ARM) || defined(__arm__)
    return Architecture::ARM;
#else
    return Architecture::Unknown;
#endif
}

const char* getArchitectureName() noexcept {
    Architecture arch = getArchitecture();
    switch (arch) {
        case Architecture::x86: return "x86";
        case Architecture::x86_64: return "x86_64";
        case Architecture::ARM: return "ARM";
        case Architecture::ARM64: return "ARM64";
        default: return "Unknown";
    }
}

Endianness getEndianness() noexcept {
    const uint32_t value = 0x01020304;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
    
    if (bytes[0] == 0x04) {
        return Endianness::Little;
    } else if (bytes[0] == 0x01) {
        return Endianness::Big;
    }
    return Endianness::Unknown;
}

bool isLittleEndian() noexcept {
    return getEndianness() == Endianness::Little;
}

bool isBigEndian() noexcept {
    return getEndianness() == Endianness::Big;
}

uint32_t getCoreCount() noexcept {
    CPUInfo info = getCPUInfo();
    return info.coreCount;
}

uint32_t getLogicalCoreCount() noexcept {
    CPUInfo info = getCPUInfo();
    return info.logicalCoreCount;
}

CPUInfo getCPUInfo() noexcept {
    CPUInfo info = {};
    info.coreCount = 1;
    info.logicalCoreCount = 1;
    info.frequencyHz = 0;
    info.vendor = "Unknown";
    info.model = "Unknown";
    info.hasSSE = false;
    info.hasSSE2 = false;
    info.hasAVX = false;
    info.hasAVX2 = false;
    info.hasAVX512 = false;
    info.hasNEON = false;

#if defined(_WIN32) || defined(_WIN64)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.logicalCoreCount = sysInfo.dwNumberOfProcessors;
    info.coreCount = sysInfo.dwNumberOfProcessors; // Windows doesn't easily distinguish physical cores
    
    // Get CPU vendor and model from registry would require more complex code
    // For now, set based on architecture
    Architecture arch = getArchitecture();
    if (arch == Architecture::x86 || arch == Architecture::x86_64) {
        info.vendor = "Unknown"; // Would need registry access
        info.model = "Unknown";
        info.hasSSE = true;
        info.hasSSE2 = true;
        // Check for AVX/AVX2 would require CPUID
    } else if (arch == Architecture::ARM || arch == Architecture::ARM64) {
        info.hasNEON = true;
    }

#elif defined(__linux__)
    info.logicalCoreCount = sysconf(_SC_NPROCESSORS_ONLN);
    info.coreCount = info.logicalCoreCount; // Simplified, would need /proc/cpuinfo for physical cores
    
    // Read /proc/cpuinfo for vendor/model (simplified)
    // For production, would parse /proc/cpuinfo properly
    
    Architecture arch = getArchitecture();
    if (arch == Architecture::ARM || arch == Architecture::ARM64) {
        info.hasNEON = true;
    }

#elif defined(__APPLE__)
    int numCores = 0;
    size_t size = sizeof(numCores);
    sysctlbyname("hw.ncpu", &numCores, &size, nullptr, 0);
    info.logicalCoreCount = numCores;
    info.coreCount = numCores;
    
    // Get CPU frequency
    int64_t freq = 0;
    size = sizeof(freq);
    sysctlbyname("hw.cpufrequency", &freq, &size, nullptr, 0);
    info.frequencyHz = freq;
    
    // Get CPU brand string
    char brand[256] = {0};
    size = sizeof(brand);
    sysctlbyname("machdep.cpu.brand_string", brand, &size, nullptr, 0);
    info.model = brand;
    
    Architecture arch = getArchitecture();
    if (arch == Architecture::ARM || arch == Architecture::ARM64) {
        info.hasNEON = true;
    }
#endif

    return info;
}

uint64_t getTotalPhysicalMemoryMB() noexcept {
    MemoryInfo info = getMemoryInfo();
    return info.totalPhysicalMB;
}

uint64_t getAvailablePhysicalMemoryMB() noexcept {
    MemoryInfo info = getMemoryInfo();
    return info.availablePhysicalMB;
}

uint64_t getPageSize() noexcept {
    MemoryInfo info = getMemoryInfo();
    return info.pageSize;
}

MemoryInfo getMemoryInfo() noexcept {
    MemoryInfo info = {};
    info.totalPhysicalMB = 0;
    info.availablePhysicalMB = 0;
    info.totalVirtualMB = 0;
    info.availableVirtualMB = 0;
    info.pageSize = 4096;

#if defined(_WIN32) || defined(_WIN64)
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus)) {
        info.totalPhysicalMB = memStatus.ullTotalPhys / (1024 * 1024);
        info.availablePhysicalMB = memStatus.ullAvailPhys / (1024 * 1024);
        info.totalVirtualMB = memStatus.ullTotalVirtual / (1024 * 1024);
        info.availableVirtualMB = memStatus.ullAvailVirtual / (1024 * 1024);
    }
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.pageSize = sysInfo.dwPageSize;

#elif defined(__linux__)
    struct sysinfo sysInfoData;
    if (sysinfo(&sysInfoData) == 0) {
        info.totalPhysicalMB = (sysInfoData.totalram * sysInfoData.mem_unit) / (1024 * 1024);
        info.availablePhysicalMB = (sysInfoData.freeram * sysInfoData.mem_unit) / (1024 * 1024);
        info.totalVirtualMB = (sysInfoData.totalswap * sysInfoData.mem_unit) / (1024 * 1024);
        info.availableVirtualMB = (sysInfoData.freeswap * sysInfoData.mem_unit) / (1024 * 1024);
    }
    
    info.pageSize = sysconf(_SC_PAGESIZE);

#elif defined(__APPLE__)
    int64_t mem = 0;
    size_t size = sizeof(mem);
    
    sysctlbyname("hw.memsize", &mem, &size, nullptr, 0);
    info.totalPhysicalMB = mem / (1024 * 1024);
    
    vm_size_t pageSize;
    host_page_size(mach_host_self(), &pageSize);
    info.pageSize = pageSize;
    
    // Get available memory (simplified)
    vm_statistics64_data_t vmStats;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmStats, &count) == KERN_SUCCESS) {
        uint64_t free = vmStats.free_count * pageSize;
        uint64_t inactive = vmStats.inactive_count * pageSize;
        info.availablePhysicalMB = (free + inactive) / (1024 * 1024);
    }
#endif

    return info;
}

std::string getOSName() {
    return getPlatformName();
}

std::string getOSVersion() {
#if defined(_WIN32) || defined(_WIN64)
    OSVERSIONINFOEX osInfo;
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    #if defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        GetVersionEx((OSVERSIONINFO*)&osInfo);
        #pragma warning(pop)
    #else
        GetVersionEx((OSVERSIONINFO*)&osInfo);
    #endif
    return std::to_string(osInfo.dwMajorVersion) + "." + std::to_string(osInfo.dwMinorVersion);
#elif defined(__linux__)
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        return std::string(unameData.release);
    }
    return "Unknown";
#elif defined(__APPLE__)
    char version[256] = {0};
    size_t size = sizeof(version);
    sysctlbyname("kern.osrelease", version, &size, nullptr, 0);
    return version;
#else
    return "Unknown";
#endif
}

std::string getHostName() {
    char hostname[256] = {0};
#if defined(_WIN32) || defined(_WIN64)
    DWORD size = sizeof(hostname);
    GetComputerNameA(hostname, &size);
#elif defined(__linux__) || defined(__APPLE__)
    gethostname(hostname, sizeof(hostname));
#endif
    return hostname;
}

std::string getUserName() {
#if defined(_WIN32) || defined(_WIN64)
    char username[256] = {0};
    DWORD size = sizeof(username);
    GetUserNameA(username, &size);
    return username;
#elif defined(__linux__) || defined(__APPLE__)
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return pw->pw_name;
    }
    return "Unknown";
#else
    return "Unknown";
#endif
}

SystemInfo getSystemInfo() noexcept {
    SystemInfo info = {};
    info.osName = getOSName();
    info.osVersion = getOSVersion();
    info.hostName = getHostName();
    info.userName = getUserName();
    info.platform = getPlatform();
    info.arch = getArchitecture();
    info.architecture = getArchitectureName();
    return info;
}

uint64_t getCurrentProcessId() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    return GetCurrentProcessId();
#elif defined(__linux__) || defined(__APPLE__)
    return getpid();
#else
    return 0;
#endif
}

uint64_t getProcessMemoryUsageMB() noexcept {
    ProcessInfo info = getProcessInfo();
    return info.memoryUsageMB;
}

ProcessInfo getProcessInfo() noexcept {
    ProcessInfo info = {};
    info.processId = getCurrentProcessId();
    info.parentId = 0;
    info.memoryUsageMB = 0;
    info.threadCount = 1;
    info.executablePath = "";
    info.workingDirectory = "";

#if defined(_WIN32) || defined(_WIN64)
    PROCESS_MEMORY_COUNTERS memInfo;
    memInfo.cb = sizeof(memInfo);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        info.memoryUsageMB = memInfo.WorkingSetSize / (1024 * 1024);
    }
    
    char path[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    info.executablePath = path;
    
    char cwd[MAX_PATH] = {0};
    GetCurrentDirectoryA(MAX_PATH, cwd);
    info.workingDirectory = cwd;

#elif defined(__linux__)
    FILE* file = fopen("/proc/self/status", "r");
    if (file) {
        char line[256] = {0};
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                unsigned long kb = 0;
                sscanf(line, "VmRSS: %lu kB", &kb);
                info.memoryUsageMB = kb / 1024;
                break;
            }
        }
        fclose(file);
    }
    
    char path[256] = {0};
    readlink("/proc/self/exe", path, sizeof(path));
    info.executablePath = path;
    
    char cwd[256] = {0};
    getcwd(cwd, sizeof(cwd));
    info.workingDirectory = cwd;

#elif defined(__APPLE__)
    struct task_basic_info taskInfo;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&taskInfo, &count) == KERN_SUCCESS) {
        info.memoryUsageMB = taskInfo.resident_size / (1024 * 1024);
    }
    
    char path[256] = {0};
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
    info.executablePath = path;
    
    char cwd[256] = {0};
    getcwd(cwd, sizeof(cwd));
    info.workingDirectory = cwd;
#endif

    return info;
}

uint64_t getCurrentThreadId() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    return GetCurrentThreadId();
#elif defined(__linux__)
    return syscall(SYS_gettid);
#elif defined(__APPLE__)
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return tid;
#else
    return 0;
#endif
}

bool setThreadName(const std::string& name) {
    // Validate name length
    if (name.empty() || name.length() > MAX_PLATFORM_STRING_LENGTH) {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows thread naming is complex, requires special handling
    // For now, return false (would need SetThreadDescription on Windows 10+)
    return false;
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), name.c_str());
    return true;
#elif defined(__APPLE__)
    pthread_setname_np(name.c_str());
    return true;
#else
    return false;
#endif
}

std::string getThreadName() {
#if defined(_WIN32) || defined(_WIN64)
    // Windows thread name retrieval is complex
    return "";
#elif defined(__linux__)
    char name[16] = {0};
    pthread_getname_np(pthread_self(), name, sizeof(name));
    return name;
#elif defined(__APPLE__)
    char name[256] = {0};
    pthread_getname_np(pthread_self(), name, sizeof(name));
    return name;
#else
    return "";
#endif
}

bool setThreadAffinity(const std::vector<uint32_t>& coreIndices) {
    if (coreIndices.empty()) {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD_PTR mask = 0;
    for (uint32_t core : coreIndices) {
        mask |= (1ULL << core);
    }
    return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;
#elif defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (uint32_t core : coreIndices) {
        CPU_SET(core, &cpuset);
    }
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
#elif defined(__APPLE__)
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = coreIndices[0]; // macOS only supports single core affinity
    return thread_policy_set(pthread_mach_thread_np(pthread_self()), 
                           (thread_policy_t)&policy, 
                           THREAD_AFFINITY_POLICY_COUNT) == KERN_SUCCESS;
#else
    return false;
#endif
}

std::string getEnv(const std::string& name) {
    if (name.empty() || name.length() > MAX_ENV_NAME_LENGTH) {
        return "";
    }

#if defined(_WIN32) || defined(_WIN64)
    char value[MAX_ENV_VALUE_LENGTH] = {0};
    DWORD size = GetEnvironmentVariableA(name.c_str(), value, sizeof(value));
    if (size == 0 || size >= sizeof(value)) {
        return "";
    }
    return value;
#else
    const char* value = getenv(name.c_str());
    if (!value) {
        return "";
    }
    return value;
#endif
}

bool setEnv(const std::string& name, const std::string& value) {
    if (name.empty() || name.length() > MAX_ENV_NAME_LENGTH) {
        return false;
    }
    
    if (value.length() > MAX_ENV_VALUE_LENGTH) {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

bool unsetEnv(const std::string& name) {
    if (name.empty() || name.length() > MAX_ENV_NAME_LENGTH) {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    return SetEnvironmentVariableA(name.c_str(), nullptr) != 0;
#else
    return unsetenv(name.c_str()) == 0;
#endif
}

} // namespace platform
} // namespace core
} // namespace poko
