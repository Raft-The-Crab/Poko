/**
 * @file allocator.h
 * @brief Core memory allocator main header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides the foundation for all memory allocation in the Poko Engine.
 * Includes the allocator interface and system allocator with thread-safe statistics,
 * guard bytes for debugging, and context-aware error handling.
 */

#ifndef POKO_CORE_MEMORY_ALLOCATOR_H
#define POKO_CORE_MEMORY_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <cassert>

namespace poko {
namespace core {
namespace memory {

// ============================================================================
// Debug Utilities
// ============================================================================

#if defined(_DEBUG) || defined(DEBUG)
    #define POKO_MEMORY_ASSERT(cond) assert(cond)
    #define POKO_MEMORY_ALWAYS_ASSERT(cond) assert(cond)
#else
    #define POKO_MEMORY_ASSERT(cond) ((void)0)
    #define POKO_MEMORY_ALWAYS_ASSERT(cond) assert(cond)
#endif

/**
 * @brief Memory allocation flags
 * 
 * These flags control allocation behavior and can be combined using bitwise operators.
 */
enum class AllocationFlags : uint32_t {
    None = 0,                  ///< No special flags
    ZeroMemory = 1 << 0,       ///< Zero allocated memory after allocation
    Aligned = 1 << 1,           ///< Request aligned allocation (use alignment parameter)
    Persistent = 1 << 2,        ///< Persistent allocation (not frame-temporary)
    Temporary = 1 << 3,         ///< Temporary allocation (frame-based, for future allocators)
    Tracked = 1 << 4,           ///< Enable memory tracking (overrides global setting)
    Debug = 1 << 5,             ///< Enable debug allocation tracking (for future debugging)
    NoThrow = 1 << 6,           ///< Return nullptr instead of throwing on failure
    Guard = 1 << 7,             ///< Add guard bytes before/after allocation for corruption detection
};

// ============================================================================
// Constants
// ============================================================================

/// Maximum allocation size to prevent overflow (leaves room for alignment padding)
constexpr size_t MAX_ALLOCATION_SIZE = SIZE_MAX - 16;

/// Default alignment for allocations (typical cache line size)
constexpr size_t DEFAULT_ALIGNMENT = 16;

/// Minimum alignment enforced for all allocations (must align pointers)
constexpr size_t MIN_ALIGNMENT = sizeof(void*);

/// Guard pattern value for detecting memory corruption (0xCD = Visual Studio debug pattern)
constexpr uint8_t GUARD_PATTERN = 0xCD;

/// Size of guard regions before and after allocations
constexpr size_t GUARD_SIZE = 8;

/// Maximum allocation size for small object optimization
constexpr size_t MAX_SMALL_ALLOCATION = 1024;

// ============================================================================
// Allocation Flags Operators
// ============================================================================

/**
 * @brief Bitwise OR for combining allocation flags
 * @param a First flag
 * @param b Second flag
 * @return Combined flags
 */
inline AllocationFlags operator|(AllocationFlags a, AllocationFlags b) noexcept {
    return static_cast<AllocationFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

/**
 * @brief Bitwise AND for testing allocation flags
 * @param a First flag
 * @param b Second flag
 * @return Combined flags
 */
inline AllocationFlags operator&(AllocationFlags a, AllocationFlags b) noexcept {
    return static_cast<AllocationFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

/**
 * @brief Bitwise XOR for toggling allocation flags
 * @param a First flag
 * @param b Second flag
 * @return Combined flags
 */
inline AllocationFlags operator^(AllocationFlags a, AllocationFlags b) noexcept {
    return static_cast<AllocationFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

/**
 * @brief Bitwise NOT for inverting allocation flags
 * @param a Flag to invert
 * @return Inverted flag
 */
inline AllocationFlags operator~(AllocationFlags a) noexcept {
    return static_cast<AllocationFlags>(~static_cast<uint32_t>(a));
}

/**
 * @brief Compound OR assignment for allocation flags
 * @param a Flag to modify
 * @param b Flag to OR with
 * @return Modified flag
 */
inline AllocationFlags& operator|=(AllocationFlags& a, AllocationFlags b) noexcept {
    a = a | b;
    return a;
}

/**
 * @brief Compound AND assignment for allocation flags
 * @param a Flag to modify
 * @param b Flag to AND with
 * @return Modified flag
 */
inline AllocationFlags& operator&=(AllocationFlags& a, AllocationFlags b) noexcept {
    a = a & b;
    return a;
}

// ============================================================================
// Statistics
// ============================================================================

/**
 * @brief Memory allocation statistics (public interface)
 * 
 * Plain struct for returning allocation statistics as a snapshot.
 * Used for monitoring and debugging memory usage.
 * 
 * All values are snapshots at the time of the call and may change
 * immediately after if allocations/deallocations continue.
 */
struct AllocationStats {
    size_t totalAllocated;      ///< Total bytes allocated over lifetime
    size_t totalFreed;          ///< Total bytes freed over lifetime
    size_t currentUsage;        ///< Current bytes in use (allocated - freed)
    size_t peakUsage;           ///< Peak bytes in use (maximum currentUsage)
    size_t allocationCount;    ///< Number of successful allocations
    size_t deallocationCount;   ///< Number of successful deallocations
    
    /**
     * @brief Get allocation efficiency (freed / allocated ratio)
     * @return Efficiency ratio (0.0 to 1.0, or 0 if no allocations)
     */
    double getEfficiency() const noexcept {
        return totalAllocated > 0 ? static_cast<double>(totalFreed) / totalAllocated : 0.0;
    }
    
    /**
     * @brief Check if there are active allocations
     * @return True if currentUsage > 0
     */
    bool hasActiveAllocations() const noexcept {
        return currentUsage > 0;
    }
};

/**
 * @brief Internal atomic statistics for thread-safe tracking
 * 
 * Used internally by allocators to track statistics with atomic operations.
 * Provides lock-free reads for performance.
 */
struct AtomicAllocationStats {
    std::atomic<size_t> totalAllocated{0};
    std::atomic<size_t> totalFreed{0};
    std::atomic<size_t> currentUsage{0};
    std::atomic<size_t> peakUsage{0};
    std::atomic<size_t> allocationCount{0};
    std::atomic<size_t> deallocationCount{0};
};

// ============================================================================
// Allocator Interface
// ============================================================================

/**
 * @brief Base allocator interface
 * 
 * All memory allocators in the engine must implement this interface.
 * Provides a unified API for different allocation strategies (system, pool, arena, etc.).
 * 
 * @section thread_safety Thread Safety
 * Implementations should be thread-safe unless documented otherwise.
 * 
 * @section implementation Implementation Guidelines
 * - Must support alignment requirements
 * - Should handle edge cases (zero size, null pointers)
 * - Should provide meaningful statistics when tracking is enabled
 * - Must follow RAII principles for resource management
 */
class IAllocator {
public:
    virtual ~IAllocator() = default;
    
    /**
     * @brief Allocate memory of given size
     * 
     * @param size Size in bytes to allocate (must not exceed MAX_ALLOCATION_SIZE)
     * @param alignment Alignment requirement in bytes (must be power of 2, >= MIN_ALIGNMENT)
     * @param flags Allocation flags controlling behavior
     * @return Pointer to allocated memory, or nullptr on failure (if NoThrow flag set)
     * @throws std::invalid_argument if size or alignment is invalid (unless NoThrow)
     * @throws std::bad_alloc if allocation fails (unless NoThrow)
     * 
     * @note Alignment is automatically enforced to at least MIN_ALIGNMENT
     * @note Size is rounded up to alignment boundary
     */
    [[nodiscard]] virtual void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT, AllocationFlags flags = AllocationFlags::None) = 0;
    
    /**
     * @brief Free previously allocated memory
     * 
     * @param ptr Pointer to memory to free (null is safe and ignored)
     * @param size Original size of allocation (optional, used for statistics)
     * 
     * @note Passing the correct size improves statistics accuracy
     * @note Some allocators may require size for proper deallocation
     */
    virtual void deallocate(void* ptr, size_t size = 0) noexcept = 0;
    
    /**
     * @brief Get allocation statistics
     * 
     * @return Current allocation statistics snapshot
     * 
     * @note Statistics may be zero if tracking is disabled
     * @note Returned values are snapshots and may change immediately
     */
    [[nodiscard]] virtual AllocationStats getStats() const noexcept = 0;
    
    /**
     * @brief Check if tracking is enabled (for fast path optimization)
     * 
     * @return True if tracking is enabled
     * 
     * @note This should be a fast operation (atomic read)
     */
    [[nodiscard]] virtual bool isTrackingEnabled() const noexcept = 0;
    
    /**
     * @brief Reset allocator state (where applicable)
     * 
     * Some allocators like linear allocators can be reset rather than freed individually.
     * For system allocator, this only resets statistics, not actual allocations.
     * 
     * @note Actual allocations are NOT freed by reset() in most allocators
     * @note Users must still deallocate all memory before allocator destruction
     */
    virtual void reset() noexcept = 0;
    
    /**
     * @brief Get allocator name for debugging
     * 
     * @return Allocator name (e.g., "SystemAllocator", "PoolAllocator")
     */
    [[nodiscard]] virtual const char* getName() const noexcept = 0;
    
    /**
     * @brief Check if allocator supports individual deallocation
     * 
     * @return True if individual deallocation is supported
     * 
     * @note Some allocators (e.g., linear) may only support bulk reset
     */
    [[nodiscard]] virtual bool supportsIndividualDeallocation() const noexcept = 0;
};

// ============================================================================
// System Allocator
// ============================================================================

/**
 * @brief Default allocator using system malloc/free
 * 
 * Thread-safe wrapper around system allocation with tracking capabilities.
 * Useful as a fallback or for general-purpose allocations where specialized
 * allocators (pool, arena, etc.) are not needed.
 * 
 * @section thread_safety Thread Safety
 * - All public methods are thread-safe using mutex protection
 * - Statistics use atomic operations for lock-free reads
 * - Tracking flag is atomic for fast-path checks
 * - Safe for concurrent allocation/deallocation from multiple threads
 * 
 * @section performance Performance Characteristics
 * - Allocation: O(1) (delegates to system allocator)
 * - Deallocation: O(1) (delegates to system allocator)
 * - Statistics: Lock-free reads, mutex-protected writes
 * - Memory overhead: Minimal (only tracking data)
 * 
 * @section guard_bytes Guard Bytes
 * When Guard flag is used, guard bytes are placed before and after the allocation:
 * - Guard pattern: 0xCD (Visual Studio debug pattern)
 * - Guard size: 8 bytes on each side
 * - Note: Current implementation requires user to remember guard flag for deallocation
 * - Future: Will store metadata header for automatic guard validation
 * 
 * @section platform Platform Notes
 * - Windows: Uses _aligned_malloc / _aligned_free
 * - POSIX: Uses posix_memalign / free
 * - Alignment is enforced to be power of 2 and >= MIN_ALIGNMENT
 */
class SystemAllocator : public IAllocator {
public:
    /**
     * @brief Constructor - initializes allocator with tracking disabled
     */
    SystemAllocator() noexcept;
    
    /**
     * @brief Destructor - cleans up allocator resources
     * 
     * @note Does NOT free any outstanding allocations
     * @note Users must deallocate all memory before destruction
     */
    ~SystemAllocator() override;
    
    /**
     * @brief Copy constructor (deleted)
     */
    SystemAllocator(const SystemAllocator&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    SystemAllocator& operator=(const SystemAllocator&) = delete;
    
    /**
     * @brief Move constructor
     */
    SystemAllocator(SystemAllocator&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    SystemAllocator& operator=(SystemAllocator&&) noexcept = default;
    
    /**
     * @copydoc IAllocator::allocate
     */
    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT, AllocationFlags flags = AllocationFlags::None) override;
    
    /**
     * @copydoc IAllocator::deallocate
     */
    void deallocate(void* ptr, size_t size = 0) noexcept override;
    
    /**
     * @copydoc IAllocator::getStats
     */
    [[nodiscard]] AllocationStats getStats() const noexcept override;
    
    /**
     * @copydoc IAllocator::reset
     * 
     * @note For SystemAllocator, reset() only clears statistics
     * @note Does NOT free any actual allocations
     */
    void reset() noexcept override;
    
    /**
     * @copydoc IAllocator::getName
     */
    [[nodiscard]] const char* getName() const noexcept override;
    
    /**
     * @copydoc IAllocator::supportsIndividualDeallocation
     */
    [[nodiscard]] bool supportsIndividualDeallocation() const noexcept override;
    
    /**
     * @copydoc IAllocator::isTrackingEnabled
     */
    [[nodiscard]] bool isTrackingEnabled() const noexcept override;
    
    /**
     * @brief Enable or disable memory tracking
     * 
     * @param enabled Whether to enable tracking
     * 
     * @note When disabled, statistics return zeros
     * @note Tracking has minimal performance overhead
     * @note Thread-safe (atomic flag)
     */
    void setTrackingEnabled(bool enabled) noexcept;
    
private:
    mutable std::mutex m_mutex;                ///< Mutex for thread safety
    AtomicAllocationStats m_stats;             ///< Internal atomic statistics
    std::atomic<bool> m_trackingEnabled;       ///< Whether tracking is enabled (atomic for fast-path reads)
};

// ============================================================================
// Global Allocator
// ============================================================================

/**
 * @brief Global system allocator instance
 * 
 * Points to the globally shared SystemAllocator instance.
 * Used throughout the engine for general-purpose allocations.
 * 
 * @warning Access should be thread-safe (see getSystemAllocator())
 */
extern SystemAllocator* g_systemAllocator;

/**
 * @brief Get global system allocator
 * 
 * Returns a reference to the globally shared SystemAllocator instance.
 * This allocator is intended for general-purpose allocations throughout the engine.
 * 
 * @return Reference to global system allocator
 * 
 * @note Thread-safe access is guaranteed
 * @note Tracking is disabled by default, enable if needed
 * @note Do not destroy this allocator - it's globally managed
 * 
 * @section usage Usage Example
 * @code
 * SystemAllocator& allocator = getSystemAllocator();
 * allocator.setTrackingEnabled(true);
 * void* ptr = allocator.allocate(1024);
 * allocator.deallocate(ptr, 1024);
 * @endcode
 */
SystemAllocator& getSystemAllocator();

} // namespace memory
} // namespace core
} // namespace poko

#endif // POKO_CORE_MEMORY_ALLOCATOR_H