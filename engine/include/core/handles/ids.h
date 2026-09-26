/**
 * @file ids.h
 * @brief Core ID types and handles for Poko Engine
 * @details Provides type-safe identifiers and generation-safe handles for object references
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace poko {
namespace core {

/**
 * @typedef InstanceID
 * @brief Unique identifier for engine instances
 * @details 64-bit unique identifier for objects in the engine
 */
using InstanceID = uint64_t;

/**
 * @typedef ResourceID
 * @brief Unique identifier for resources (textures, meshes, etc.)
 */
using ResourceID = uint64_t;

/**
 * @typedef AssetID
 * @brief Unique identifier for loaded assets
 */
using AssetID = uint64_t;

/**
 * @typedef ComponentID
 * @brief Unique identifier for components
 */
using ComponentID = uint64_t;

/**
 * @typedef UserID
 * @brief Unique identifier for users/players
 */
using UserID = uint64_t;

/**
 * @typedef SessionID
 * @brief Unique identifier for sessions
 */
using SessionID = uint64_t;

// Use different underlying types for StrongID to ensure type safety
using InstanceIDUnderlying = uint64_t;
using ResourceIDUnderlying = uint32_t;
using AssetIDUnderlying = uint32_t;
using ComponentIDUnderlying = uint32_t;

/**
 * @brief Invalid ID constant
 */
constexpr InstanceID INVALID_INSTANCE_ID = 0;
constexpr ResourceID INVALID_RESOURCE_ID = 0;
constexpr AssetID INVALID_ASSET_ID = 0;
constexpr ComponentID INVALID_COMPONENT_ID = 0;
constexpr UserID INVALID_USER_ID = 0;
constexpr SessionID INVALID_SESSION_ID = 0;

/**
 * @class IDGenerator
 * @brief Generates unique IDs for various engine entities
 * @details Thread-safe ID generation with configurable ranges
 */
class IDGenerator {
public:
    /**
     * @brief Construct an ID generator
     * @param start Starting ID value
     */
    explicit IDGenerator(uint64_t start = 1);

    /**
     * @brief Generate a new unique ID
     * @return New unique ID
     */
    uint64_t generate();

    /**
     * @brief Reserve a specific ID
     * @param id ID to reserve
     * @return true if ID was successfully reserved
     */
    bool reserve(uint64_t id);

    /**
     * @brief Get the current maximum ID
     * @return Current maximum ID
     */
    uint64_t get_max() const { return max_id_; }

    /**
     * @brief Reset the generator
     * @param start New starting value
     */
    void reset(uint64_t start = 1);

private:
    uint64_t next_id_;
    uint64_t max_id_;
};

/**
 * @class Handle
 * @brief Generation-safe handle for object references
 * @details Prevents use-after-free by including a generation counter
 * @tparam IDType The underlying ID type
 */
template<typename IDType>
class Handle {
public:
    /**
     * @brief Default constructor (invalid handle)
     */
    Handle() : id_(0), generation_(0) {}

    /**
     * @brief Construct a handle from ID and generation
     * @param id The object ID
     * @param generation The generation counter
     */
    Handle(IDType id, uint32_t generation)
        : id_(id), generation_(generation) {}

    /**
     * @brief Get the ID
     * @return The object ID
     */
    IDType id() const { return id_; }

    /**
     * @brief Get the generation
     * @return The generation counter
     */
    uint32_t generation() const { return generation_; }

    /**
     * @brief Check if handle is valid
     * @return true if handle has valid ID
     */
    bool is_valid() const { return id_ != 0; }

    /**
     * @brief Check if two handles are equal
     * @param other Other handle
     * @return true if handles are equal
     */
    bool operator==(const Handle& other) const {
        return id_ == other.id_ && generation_ == other.generation_;
    }

    /**
     * @brief Check if two handles are not equal
     * @param other Other handle
     * @return true if handles are not equal
     */
    bool operator!=(const Handle& other) const {
        return !(*this == other);
    }

    /**
     * @brief Convert to string representation
     * @return String representation
     */
    std::string to_string() const {
        return std::to_string(id_) + ":" + std::to_string(generation_);
    }

private:
    IDType id_;
    uint32_t generation_;
};

/**
 * @typedef InstanceHandle
 * @brief Handle for instance references
 */
using InstanceHandle = Handle<InstanceIDUnderlying>;

/**
 * @typedef ResourceHandle
 * @brief Handle for resource references
 */
using ResourceHandle = Handle<ResourceIDUnderlying>;

/**
 * @typedef AssetHandle
 * @brief Handle for asset references
 */
using AssetHandle = Handle<AssetIDUnderlying>;

/**
 * @typedef ComponentHandle
 * @brief Handle for component references
 */
using ComponentHandle = Handle<ComponentIDUnderlying>;

/**
 * @class HandleTable
 * @brief Table for managing handles with generation counters
 * @details Tracks object lifecycles and prevents use-after-free
 * @tparam IDType The underlying ID type
 */
template<typename IDType>
class HandleTable {
public:
    /**
     * @brief Maximum number of handles in the table
     */
    static constexpr size_t MAX_HANDLES = 65536;

    /**
     * @brief Construct a handle table
     */
    HandleTable();

    /**
     * @brief Allocate a new handle
     * @param id The object ID
     * @return Handle for the object
     */
    Handle<IDType> allocate(IDType id);

    /**
     * @brief Deallocate a handle
     * @param handle Handle to deallocate
     * @return true if deallocation succeeded
     */
    bool deallocate(const Handle<IDType>& handle);

    /**
     * @brief Check if a handle is still valid
     * @param handle Handle to check
     * @return true if handle is valid
     */
    bool is_valid(const Handle<IDType>& handle) const;

    /**
     * @brief Get the number of active handles
     * @return Number of active handles
     */
    size_t active_count() const { return active_count_; }

    /**
     * @brief Clear all handles
     */
    void clear();

private:
    struct Entry {
        IDType id;
        uint32_t generation;
        bool active;
    };

    Entry entries_[MAX_HANDLES];
    size_t active_count_;
    size_t next_free_;
};

/**
 * @class StrongID
 * @brief Strongly-typed ID wrapper for type safety
 * @tparam Tag Type tag for type safety
 * @tparam UnderlyingType Underlying numeric type
 */
template<typename Tag, typename UnderlyingType = uint64_t>
class StrongID {
public:
    /**
     * @brief Default constructor (invalid ID)
     */
    StrongID() : value_(0) {}

    /**
     * @brief Construct from underlying value
     * @param value The underlying value
     */
    explicit StrongID(UnderlyingType value) : value_(value) {}

    /**
     * @brief Get the underlying value
     * @return The underlying value
     */
    UnderlyingType value() const { return value_; }

    /**
     * @brief Check if ID is valid
     * @return true if ID is not zero
     */
    bool is_valid() const { return value_ != 0; }

    /**
     * @brief Compare IDs
     * @param other Other ID
     * @return true if IDs are equal
     */
    bool operator==(const StrongID& other) const {
        return value_ == other.value_;
    }

    /**
     * @brief Compare IDs
     * @param other Other ID
     * @return true if IDs are not equal
     */
    bool operator!=(const StrongID& other) const {
        return !(*this == other);
    }

    /**
     * @brief Compare IDs for ordering
     * @param other Other ID
     * @return true if this ID is less than other
     */
    bool operator<(const StrongID& other) const {
        return value_ < other.value_;
    }

    /**
     * @brief Convert to string
     * @return String representation
     */
    std::string to_string() const {
        return std::to_string(value_);
    }

private:
    UnderlyingType value_;
};

// Type tags for StrongID
struct InstanceIDTag {};
struct ResourceIDTag {};
struct ComponentIDTag {};
struct AssetIDTag {};

/**
 * @typedef TypedInstanceID
 * @brief Strongly-typed instance ID
 */
using TypedInstanceID = StrongID<InstanceIDTag, InstanceIDUnderlying>;

/**
 * @typedef TypedResourceID
 * @brief Strongly-typed resource ID
 */
using TypedResourceID = StrongID<ResourceIDTag, ResourceIDUnderlying>;

/**
 * @typedef TypedComponentID
 * @brief Strongly-typed component ID
 */
using TypedComponentID = StrongID<ComponentIDTag, ComponentIDUnderlying>;

/**
 * @typedef TypedAssetID
 * @brief Strongly-typed asset ID
 */
using TypedAssetID = StrongID<AssetIDTag, AssetIDUnderlying>;

} // namespace core
} // namespace poko
