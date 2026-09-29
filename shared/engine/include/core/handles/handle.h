/**
 * @file handle.h
 * @brief Core handle system main header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides generation-aware handles for safe object references in the Poko Engine.
 * Handles prevent stale references by including generation counters that increment when
 * objects are destroyed, making old handles invalid.
 */

#ifndef POKO_CORE_HANDLES_HANDLE_H
#define POKO_CORE_HANDLES_HANDLE_H

#include <cstdint>
#include <functional>

namespace poko {
namespace core {
namespace handles {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Type for handle index
 * 
 * The index identifies which slot in the handle manager this handle refers to.
 * Indices are reused when handles are freed, but generations are incremented.
 */
using HandleIndex = uint32_t;

/**
 * @brief Type for handle generation
 * 
 * The generation prevents stale references. When a handle is freed and its
 * index is reused, the generation is incremented, making old handles invalid.
 */
using HandleGeneration = uint32_t;

// ============================================================================
// Constants
// ============================================================================

/// Maximum index value for handles (24 bits, leaving 8 bits for metadata in future)
constexpr HandleIndex MAX_HANDLE_INDEX = 0xFFFFFF;

/// Invalid handle index constant (used for null handles)
constexpr HandleIndex INVALID_HANDLE_INDEX = MAX_HANDLE_INDEX + 1;

/// Maximum generation value (will wrap after 2^32 allocations/frees)
constexpr HandleGeneration MAX_GENERATION = 0xFFFFFFFF;

/// Maximum handle manager capacity (for safety)
constexpr size_t MAX_HANDLE_CAPACITY = 1000000;

// ============================================================================
// Handle Structure
// ============================================================================

/**
 * @brief Strongly-typed handle with generation checking
 * 
 * Handles consist of an index and generation to prevent stale references.
 * When an object is destroyed, its generation is incremented, making
 * old handles invalid.
 * 
 * @section memory_layout Memory Layout
 * - Total size: 8 bytes (2 x uint32_t)
 * - Index: Lower 24 bits (up to 16,777,215 handles)
 * - Generation: Full 32 bits (wraps after 4 billion allocations)
 * 
 * @section thread_safety Thread Safety
 * Handle itself is trivially copyable and thread-safe for reads.
 * Validation requires synchronization with HandleManager.
 * 
 * @section performance Performance
 * - Copy/move: O(1) (trivial)
 * - Comparison: O(1) (integer comparison)
 * - Hash: O(1) (bitwise operations)
 * - Size: 8 bytes (compact)
 */
struct Handle {
    HandleIndex index;           ///< Index into handle table
    HandleGeneration generation; ///< Generation counter for stale reference prevention
    
    /**
     * @brief Default constructor - creates invalid handle
     */
    constexpr Handle() noexcept
        : index(INVALID_HANDLE_INDEX)
        , generation(0)
    {}
    
    /**
     * @brief Construct handle from index and generation
     * 
     * @param idx Handle index
     * @param gen Handle generation
     */
    constexpr Handle(HandleIndex idx, HandleGeneration gen) noexcept
        : index(idx)
        , generation(gen)
    {}
    
    /**
     * @brief Check if handle is valid (not null)
     * 
     * @return True if handle has a valid index
     * 
     * @note This only checks the index, not generation matching
     * @note Use HandleManager::isValid() for full validation
     */
    constexpr bool isValid() const noexcept {
        return index != INVALID_HANDLE_INDEX;
    }
    
    /**
     * @brief Check if handle is null (invalid)
     * 
     * @return True if handle is null/invalid
     */
    constexpr bool isNull() const noexcept {
        return !isValid();
    }
    
    /**
     * @brief Get null/invalid handle
     * 
     * @return Null handle constant
     */
    static constexpr Handle null() noexcept {
        return Handle();
    }
    
    /**
     * @brief Equality operator
     * 
     * @param other Handle to compare with
     * @return True if both index and generation match
     */
    constexpr bool operator==(const Handle& other) const noexcept {
        return index == other.index && generation == other.generation;
    }
    
    /**
     * @brief Inequality operator
     * 
     * @param other Handle to compare with
     * @return True if index or generation differ
     */
    constexpr bool operator!=(const Handle& other) const noexcept {
        return !(*this == other);
    }
    
    /**
     * @brief Less than operator (for sorting)
     * 
     * Compares by index first, then generation.
     * 
     * @param other Handle to compare with
     * @return True if this handle is less than other
     */
    constexpr bool operator<(const Handle& other) const noexcept {
        if (index != other.index) return index < other.index;
        return generation < other.generation;
    }
    
    /**
     * @brief Less than or equal operator
     * 
     * @param other Handle to compare with
     * @return True if this handle is <= other
     */
    constexpr bool operator<=(const Handle& other) const noexcept {
        return !(other < *this);
    }
    
    /**
     * @brief Greater than operator
     * 
     * @param other Handle to compare with
     * @return True if this handle is greater than other
     */
    constexpr bool operator>(const Handle& other) const noexcept {
        return other < *this;
    }
    
    /**
     * @brief Greater than or equal operator
     * 
     * @param other Handle to compare with
     * @return True if this handle is >= other
     */
    constexpr bool operator>=(const Handle& other) const noexcept {
        return !(*this < other);
    }
};

// ============================================================================
// Hash Function
// ============================================================================

/**
 * @brief Hash function for Handle (for use with unordered_map/set)
 * 
 * Combines index and generation into a single hash value using
 * bitwise operations for good distribution.
 * 
 * @section algorithm Algorithm
 * Uses XOR with bit shift to combine the two 32-bit values:
 * - hash = index ^ (generation << 16)
 * 
 * This provides good distribution while being fast to compute.
 */
struct HandleHash {
    /**
     * @brief Compute hash for a handle
     * 
     * @param handle Handle to hash
     * @return Hash value
     */
    std::size_t operator()(const Handle& handle) const noexcept {
        // Combine index and generation into a single hash
        // Shift generation by 16 bits to avoid overlapping with index
        return static_cast<std::size_t>(handle.index) ^ 
               (static_cast<std::size_t>(handle.generation) << 16);
    }
};

} // namespace handles
} // namespace core
} // namespace poko

#endif // POKO_CORE_HANDLES_HANDLE_H
