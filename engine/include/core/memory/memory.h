/**
 * @file memory.h
 * @brief Memory utilities and allocators for Poko Engine
 * @details Provides custom allocators, memory pools, and memory tracking
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <functional>

namespace poko {
namespace core {
namespace memory {

/**
 * @brief Align a size to the specified alignment
 * @param size Size to align
 * @param alignment Alignment boundary (must be power of 2)
 * @return Aligned size
 */
constexpr size_t align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief Align a pointer to the specified alignment
 * @param ptr Pointer to align
 * @param alignment Alignment boundary (must be power of 2)
 * @return Aligned pointer
 */
constexpr void* align_pointer(void* ptr, size_t alignment) {
    return reinterpret_cast<void*>(
        (reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1)
    );
}

/**
 * @brief Check if a pointer is aligned to the specified alignment
 * @param ptr Pointer to check
 * @param alignment Alignment boundary (must be power of 2)
 * @return true if pointer is aligned
 */
constexpr bool is_aligned(void* ptr, size_t alignment) {
    return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
}

/**
 * @brief Default alignment for SIMD operations
 */
constexpr size_t DEFAULT_ALIGNMENT = 16;

/**
 * @brief Alignment for cache lines
 */
constexpr size_t CACHE_LINE_ALIGNMENT = 64;

/**
 * @enum MemoryCategory
 * @brief Categories for memory tracking
 */
enum class MemoryCategory {
    GENERAL,
    RENDERING,
    PHYSICS,
    AUDIO,
    NETWORKING,
    SCRIPTING,
    ASSETS,
    TEMPORARY,
    COUNT
};

/**
 * @class MemoryStats
 * @brief Statistics for memory tracking
 */
struct MemoryStats {
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t deallocation_count;
};

/**
 * @class MemoryTracker
 * @brief Global memory tracking system
 */
class MemoryTracker {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the memory tracker
     */
    static MemoryTracker& instance();

    /**
     * @brief Record an allocation
     * @param size Size of allocation
     * @param category Memory category
     */
    void record_allocation(size_t size, MemoryCategory category);

    /**
     * @brief Record a deallocation
     * @param size Size of deallocation
     * @param category Memory category
     */
    void record_deallocation(size_t size, MemoryCategory category);

    /**
     * @brief Get statistics for a category
     * @param category Memory category
     * @return Memory statistics
     */
    MemoryStats get_stats(MemoryCategory category) const;

    /**
     * @brief Get total statistics across all categories
     * @return Total memory statistics
     */
    MemoryStats get_total_stats() const;

    /**
     * @brief Reset statistics
     */
    void reset();

    /**
     * @brief Enable or disable tracking
     * @param enabled true to enable tracking
     */
    void set_enabled(bool enabled) { enabled_ = enabled; }

    /**
     * @brief Check if tracking is enabled
     * @return true if tracking is enabled
     */
    bool is_enabled() const { return enabled_; }

private:
    MemoryTracker();
    ~MemoryTracker() = default;

    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;

    MemoryStats stats_[static_cast<size_t>(MemoryCategory::COUNT)];
    bool enabled_;
};

/**
 * @class Allocator
 * @brief Base interface for custom allocators
 */
class Allocator {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Allocator() = default;

    /**
     * @brief Allocate memory
     * @param size Size to allocate
     * @param alignment Alignment requirement
     * @return Pointer to allocated memory, or nullptr on failure
     */
    virtual void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) = 0;

    /**
     * @brief Free memory
     * @param ptr Pointer to free
     * @param size Size of the allocation (for some allocators)
     */
    virtual void free(void* ptr, size_t size = 0) = 0;

    /**
     * @brief Get the total allocated size
     * @return Total allocated size in bytes
     */
    virtual size_t get_total_allocated() const = 0;

    /**
     * @brief Get the allocation count
     * @return Number of active allocations
     */
    virtual size_t get_allocation_count() const = 0;
};

/**
 * @class MallocAllocator
 * @brief Wrapper around standard malloc/free with tracking
 */
class MallocAllocator : public Allocator {
public:
    /**
     * @brief Construct a malloc allocator
     * @param category Memory category for tracking
     */
    explicit MallocAllocator(MemoryCategory category = MemoryCategory::GENERAL);

    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) override;
    void free(void* ptr, size_t size = 0) override;
    size_t get_total_allocated() const override { return total_allocated_; }
    size_t get_allocation_count() const override { return allocation_count_; }

private:
    MemoryCategory category_;
    size_t total_allocated_;
    size_t allocation_count_;
};

/**
 * @class PoolAllocator
 * @brief Fixed-size pool allocator for fast allocation of same-sized objects
 */
class PoolAllocator : public Allocator {
public:
    /**
     * @brief Construct a pool allocator
     * @param object_size Size of each object in the pool
     * @param object_count Number of objects in the pool
     * @param category Memory category for tracking
     */
    PoolAllocator(size_t object_size, size_t object_count,
                 MemoryCategory category = MemoryCategory::GENERAL);

    ~PoolAllocator();

    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) override;
    void free(void* ptr, size_t size = 0) override;
    size_t get_total_allocated() const override { return total_allocated_; }
    size_t get_allocation_count() const override { return allocation_count_; }

    /**
     * @brief Get the number of free slots
     * @return Number of free slots in the pool
     */
    size_t get_free_count() const { return free_count_; }

    /**
     * @brief Reset the pool (free all allocations)
     */
    void reset();

private:
    struct FreeNode {
        FreeNode* next;
    };

    void* memory_;
    FreeNode* free_list_;
    size_t object_size_;
    size_t object_count_;
    size_t free_count_;
    size_t total_allocated_;
    size_t allocation_count_;
    MemoryCategory category_;
};

/**
 * @class StackAllocator
 * @brief Linear allocator for temporary allocations
 * @details Allocations are freed in LIFO order or by resetting the entire stack
 */
class StackAllocator : public Allocator {
public:
    /**
     * @brief Construct a stack allocator
     * @param size Total size of the stack
     * @param category Memory category for tracking
     */
    explicit StackAllocator(size_t size, MemoryCategory category = MemoryCategory::GENERAL);

    ~StackAllocator();

    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT) override;
    void free(void* ptr, size_t size = 0) override;
    size_t get_total_allocated() const override { return current_ptr_ - start_ptr_; }
    size_t get_allocation_count() const override { return allocation_count_; }

    /**
     * @brief Get a marker for the current stack position
     * @return Current stack marker
     */
    size_t get_marker() const { return current_ptr_ - start_ptr_; }

    /**
     * @brief Free allocations to a marker
     * @param marker Stack marker to free to
     */
    void free_to_marker(size_t marker);

    /**
     * @brief Reset the entire stack
     */
    void reset();

    /**
     * @brief Get the remaining free space
     * @return Remaining bytes in the stack
     */
    size_t get_remaining() const { return end_ptr_ - current_ptr_; }

private:
    struct AllocationHeader {
        size_t offset;
        size_t alignment;
    };

    void* memory_;
    uint8_t* start_ptr_;
    uint8_t* current_ptr_;
    uint8_t* end_ptr_;
    size_t allocation_count_;
    MemoryCategory category_;
};

/**
 * @class ScopedAllocation
 * @brief RAII wrapper for stack allocator allocations
 * @tparam AllocatorType Type of allocator to use
 */
template<typename AllocatorType>
class ScopedAllocation {
public:
    /**
     * @brief Construct a scoped allocation
     * @param allocator Reference to the allocator
     * @param size Size to allocate
     * @param alignment Alignment requirement
     */
    ScopedAllocation(AllocatorType& allocator, size_t size, size_t alignment = DEFAULT_ALIGNMENT)
        : allocator_(allocator), ptr_(allocator.allocate(size, alignment)), size_(size) {}

    /**
     * @brief Destructor - frees the allocation
     */
    ~ScopedAllocation() {
        if (ptr_) {
            allocator_.free(ptr_, size_);
        }
    }

    /**
     * @brief Get the allocated pointer
     * @return Pointer to allocated memory
     */
    void* get() const { return ptr_; }

    /**
     * @brief Release ownership of the pointer
     * @return The pointer (caller becomes responsible for freeing)
     */
    void* release() {
        void* ptr = ptr_;
        ptr_ = nullptr;
        return ptr;
    }

    // Prevent copying
    ScopedAllocation(const ScopedAllocation&) = delete;
    ScopedAllocation& operator=(const ScopedAllocation&) = delete;

private:
    AllocatorType& allocator_;
    void* ptr_;
    size_t size_;
};

/**
 * @brief Helper function to allocate memory with category tracking
 * @param size Size to allocate
 * @param category Memory category
 * @return Pointer to allocated memory
 */
void* allocate(size_t size, MemoryCategory category = MemoryCategory::GENERAL);

/**
 * @brief Helper function to free memory with category tracking
 * @param ptr Pointer to free
 * @param size Size of the allocation
 * @param category Memory category
 */
void free(void* ptr, size_t size, MemoryCategory category = MemoryCategory::GENERAL);

} // namespace memory
} // namespace core
} // namespace poko
