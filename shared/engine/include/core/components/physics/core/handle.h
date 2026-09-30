/**
 * @file handle.h
 * @brief Generation-safe handles for physics objects
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CORE_HANDLE_H
#define POKO_CORE_COMPONENTS_PHYSICS_CORE_HANDLE_H

#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace core {

/**
 * @brief Invalid handle constant
 */
static constexpr uint32_t INVALID_HANDLE_INDEX = 0xFFFFFFFF;
static constexpr uint32_t INVALID_HANDLE_GENERATION = 0;

/**
 * @brief Generation-safe handle base
 *
 * Handles contain index and generation to prevent stale handle issues.
 * Index refers to the slot in a pool/array.
 * Generation increments when a slot is reused, invalidating old handles.
 */
struct Handle {
    uint32_t index;
    uint32_t generation;

    /**
     * @brief Default constructor (invalid handle)
     */
    constexpr Handle() noexcept
        : index(INVALID_HANDLE_INDEX)
        , generation(INVALID_HANDLE_GENERATION) {}

    /**
     * @brief Construct from index and generation
     */
    constexpr Handle(uint32_t index_, uint32_t generation_) noexcept
        : index(index_)
        , generation(generation_) {}

    /**
     * @brief Check if handle is valid
     */
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return index != INVALID_HANDLE_INDEX;
    }

    /**
     * @brief Check equality
     */
    [[nodiscard]] constexpr bool operator==(const Handle& other) const noexcept {
        return index == other.index && generation == other.generation;
    }

    /**
     * @brief Check inequality
     */
    [[nodiscard]] constexpr bool operator!=(const Handle& other) const noexcept {
        return !(*this == other);
    }

    /**
     * @brief Get index only
     */
    [[nodiscard]] constexpr uint32_t getIndex() const noexcept {
        return index;
    }

    /**
     * @brief Get generation only
     */
    [[nodiscard]] constexpr uint32_t getGeneration() const noexcept {
        return generation;
    }

    /**
     * @brief Check if generation matches expected (for validation)
     */
    [[nodiscard]] constexpr bool generationMatches(uint32_t expected) const noexcept {
        return generation == expected;
    }

    /**
     * @brief Create invalid handle
     */
    [[nodiscard]] static constexpr Handle invalid() noexcept {
        return Handle();
    }
};

/**
 * @brief Rigid body handle
 */
struct BodyHandle : public Handle {
    using Handle::Handle;
    constexpr BodyHandle() noexcept : Handle() {}
    constexpr BodyHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Collider handle
 */
struct ColliderHandle : public Handle {
    using Handle::Handle;
    constexpr ColliderHandle() noexcept : Handle() {}
    constexpr ColliderHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Shape handle
 */
struct ShapeHandle : public Handle {
    using Handle::Handle;
    constexpr ShapeHandle() noexcept : Handle() {}
    constexpr ShapeHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Constraint handle
 */
struct ConstraintHandle : public Handle {
    using Handle::Handle;
    constexpr ConstraintHandle() noexcept : Handle() {}
    constexpr ConstraintHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Joint handle (constraint subclass)
 */
struct JointHandle : public ConstraintHandle {
    using ConstraintHandle::ConstraintHandle;
    constexpr JointHandle() noexcept : ConstraintHandle() {}
    constexpr JointHandle(uint32_t index, uint32_t generation) noexcept : ConstraintHandle(index, generation) {}
};

/**
 * @brief Broadphase proxy handle
 */
struct BroadphaseProxyHandle : public Handle {
    using Handle::Handle;
    constexpr BroadphaseProxyHandle() noexcept : Handle() {}
    constexpr BroadphaseProxyHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Material handle
 */
struct MaterialHandle : public Handle {
    using Handle::Handle;
    constexpr MaterialHandle() noexcept : Handle() {}
    constexpr MaterialHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

/**
 * @brief Island handle
 */
struct IslandHandle : public Handle {
    using Handle::Handle;
    constexpr IslandHandle() noexcept : Handle() {}
    constexpr IslandHandle(uint32_t index, uint32_t generation) noexcept : Handle(index, generation) {}
};

} // namespace core
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CORE_HANDLE_H
