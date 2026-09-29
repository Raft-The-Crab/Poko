/**
 * @file handle_manager.h
 * @brief Handle manager for allocating and tracking handles
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides the HandleManager class for allocating handles
 * and tracking object generations to prevent stale references.
 */

#ifndef POKO_CORE_HANDLES_HANDLE_MANAGER_H
#define POKO_CORE_HANDLES_HANDLE_MANAGER_H

#include "core/handles/handle.h"
#include <vector>
#include <mutex>
#include <atomic>

namespace poko {
namespace core {
namespace handles {

// ============================================================================
// Constants
// ============================================================================

/**
 * @brief Maximum number of handles a manager can allocate
 * 
 * Limits handle table size to prevent unbounded memory growth.
 * Can be adjusted based on application requirements.
 */
constexpr size_t MAX_HANDLES = 1024 * 1024; // 1 million handles

// ============================================================================
// Statistics
// ============================================================================

/**
 * @brief Handle manager statistics
 * 
 * Provides snapshot of handle manager state for monitoring and debugging.
 */
struct HandleManagerStats {
    size_t totalAllocated;      ///< Total handles allocated over lifetime
    size_t totalFreed;          ///< Total handles freed over lifetime
    size_t activeHandles;       ///< Currently active handles
    size_t freeListSize;        ///< Size of free list for reuse
    
    /**
     * @brief Get handle utilization ratio
     * @return Ratio of active handles to capacity (0.0 to 1.0)
     */
    double getUtilization() const noexcept {
        return static_cast<double>(activeHandles) / MAX_HANDLES;
    }
    
    /**
     * @brief Get free list efficiency
     * @return Ratio of freed handles in free list (0.0 to 1.0)
     */
    double getFreeListEfficiency() const noexcept {
        return totalFreed > 0 ? static_cast<double>(freeListSize) / totalFreed : 0.0;
    }
};

// ============================================================================
// Internal Structures
// ============================================================================

/**
 * @brief Internal handle entry
 * 
 * Stores the generation and active state for each handle index.
 * Used internally by HandleManager.
 */
struct HandleEntry {
    HandleGeneration generation;  ///< Current generation for this index
    bool active;                  ///< Whether this handle is currently allocated
};

// ============================================================================
// Handle Manager
// ============================================================================

/**
 * @brief Manages handle allocation and generation tracking
 * 
 * Thread-safe handle allocator that reuses indices with generation
 * increments to prevent stale references.
 * 
 * @section allocation_strategy Allocation Strategy
 * 1. Try to reuse an index from the free list (previously freed handles)
 * 2. If free list is empty, allocate a new index (up to MAX_HANDLES)
 * 3. When reusing, increment the generation to invalidate old handles
 * 
 * @section thread_safety Thread Safety
 * - All public methods are thread-safe using mutex protection
 * - Statistics use atomic operations for lock-free reads
 * - Safe for concurrent allocation/deallocation from multiple threads
 * 
 * @section performance Performance Characteristics
 * - Allocation: O(1) average (O(1) with free list, O(1) for new allocation)
 * - Deallocation: O(1) (push to free list)
 * - Validation: O(1) (index lookup + generation compare)
 * - Memory: ~12 bytes per handle (HandleEntry)
 * 
 * @section memory_layout Memory Layout
 * - m_entries: Vector of HandleEntry (one per allocated index)
 * - m_freeList: Vector of freed indices for reuse
 * - Statistics: Atomic counters for tracking
 */
class HandleManager {
public:
    /**
     * @brief Constructor - initializes empty handle manager
     */
    HandleManager();
    
    /**
     * @brief Destructor - cleans up handle manager
     * 
     * @note Does NOT invalidate any outstanding handles
     * @note Users should free all handles before destruction
     */
    ~HandleManager();
    
    /**
     * @brief Allocate a new handle
     * 
     * Either reuses a freed index from the free list (with incremented generation)
     * or allocates a new index if the free list is empty.
     * 
     * @return Newly allocated handle
     * @throws std::runtime_error if capacity (MAX_HANDLES) is exceeded
     * 
     * @note Thread-safe
     * @note Generation starts at 1 (0 is unused)
     */
    [[nodiscard]] Handle allocate();
    
    /**
     * @brief Free a handle (increment generation)
     * 
     * Marks the handle as inactive and adds its index to the free list.
     * The generation is NOT incremented here - it's incremented on next allocation.
     * 
     * @param handle Handle to free
     * @return True if handle was valid and freed, false otherwise
     * 
     * @note Thread-safe
     * @note Double-free is safe (returns false on second attempt)
     * @note Freeing an invalid handle returns false
     */
    [[nodiscard]] bool free(Handle handle);
    
    /**
     * @brief Check if a handle is currently valid
     * 
     * Validates that:
     * - Handle index is within bounds
     * - Handle generation matches current generation for that index
     * - Handle is currently active (not freed)
     * 
     * @param handle Handle to check
     * @return True if handle is valid (active with matching generation)
     * 
     * @note Thread-safe
     * @note Stale handles (wrong generation) return false
     */
    [[nodiscard]] bool isValid(Handle handle) const;
    
    /**
     * @brief Get generation for a handle index
     * 
     * @param index Handle index
     * @return Current generation for this index (0 if index out of bounds)
     * 
     * @note Thread-safe
     * @note Useful for debugging handle state
     */
    [[nodiscard]] HandleGeneration getGeneration(HandleIndex index) const;
    
    /**
     * @brief Get manager statistics
     * 
     * @return Current statistics snapshot
     * 
     * @note Thread-safe (lock-free reads)
     * @note Statistics are snapshots and may change immediately
     */
    [[nodiscard]] HandleManagerStats getStats() const;
    
    /**
     * @brief Reset the handle manager (clear all handles)
     * 
     * Clears all entries, empties the free list, and resets statistics.
     * All outstanding handles become invalid.
     * 
     * @note Thread-safe
     * @warning This invalidates ALL handles, even active ones
     * @warning Use with caution - ensure no handles are in use
     */
    void reset();
    
    /**
     * @brief Get the maximum capacity of the manager
     * 
     * @return Maximum number of handles (MAX_HANDLES)
     * 
     * @note Thread-safe (constant)
     */
    [[nodiscard]] size_t getCapacity() const;
    
    /**
     * @brief Get the current number of active handles
     * 
     * @return Current active handle count
     * 
     * @note Thread-safe (atomic read)
     */
    [[nodiscard]] size_t getActiveCount() const;
    
private:
    mutable std::mutex m_mutex;                   ///< Mutex for thread safety
    std::vector<HandleEntry> m_entries;          ///< Handle entries (one per index)
    std::vector<HandleIndex> m_freeList;        ///< Free list for index reuse
    std::atomic<size_t> m_activeCount;           ///< Current active handle count
    std::atomic<size_t> m_totalAllocated;        ///< Total handles allocated (lifetime)
    std::atomic<size_t> m_totalFreed;            ///< Total handles freed (lifetime)
};

// ============================================================================
// Global Handle Manager
// ============================================================================

/**
 * @brief Global handle manager instance
 * 
 * Points to the globally shared HandleManager instance.
 * Used throughout the engine for general-purpose handle allocation.
 * 
 * @warning Access should be thread-safe (see getHandleManager())
 */
extern HandleManager* g_handleManager;

/**
 * @brief Get global handle manager
 * 
 * Returns a reference to the globally shared HandleManager instance.
 * This manager is intended for general-purpose handle allocation throughout the engine.
 * 
 * @return Reference to global handle manager
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this manager - it's globally managed
 * 
 * @section usage Usage Example
 * @code
 * HandleManager& manager = getHandleManager();
 * Handle handle = manager.allocate();
 * // Use handle...
 * manager.free(handle);
 * @endcode
 */
HandleManager& getHandleManager();

} // namespace handles
} // namespace core
} // namespace poko

#endif // POKO_CORE_HANDLES_HANDLE_MANAGER_H
