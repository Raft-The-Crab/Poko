/**
 * @file material.h
 * @brief Physics material
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATERIALS_MATERIAL_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATERIALS_MATERIAL_H

#include "../core/handle.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace materials {

using core::MaterialHandle;

/**
 * @brief Material combination rule
 */
enum class MaterialCombineRule : uint32_t {
    Average,
    Minimum,
    Maximum,
    Multiply
};

/**
 * @brief Physics material
 */
class Material {
public:
    MaterialHandle handle;
    float friction;
    float restitution;
    float rollingResistance;
    float density;
    MaterialCombineRule frictionCombineRule;
    MaterialCombineRule restitutionCombineRule;
    
    /**
     * @brief Constructor
     */
    Material() noexcept
        : handle()
        , friction(0.5f)
        , restitution(0.0f)
        , rollingResistance(0.0f)
        , density(1.0f)
        , frictionCombineRule(MaterialCombineRule::Multiply)
        , restitutionCombineRule(MaterialCombineRule::Maximum) {}

    /**
     * @brief Constructor with parameters
     */
    Material(float friction_, float restitution_, float density_ = 1.0f) noexcept;
    
    /**
     * @brief Combine friction with another material
     */
    [[nodiscard]] float combineFriction(float otherFriction) const noexcept {
        switch (frictionCombineRule) {
            case MaterialCombineRule::Average:
                return (friction + otherFriction) * 0.5f;
            case MaterialCombineRule::Minimum:
                return friction < otherFriction ? friction : otherFriction;
            case MaterialCombineRule::Maximum:
                return friction > otherFriction ? friction : otherFriction;
            case MaterialCombineRule::Multiply:
                return friction * otherFriction;
            default:
                return friction;
        }
    }
    
    /**
     * @brief Combine restitution with another material
     */
    [[nodiscard]] float combineRestitution(float otherRestitution) const noexcept {
        switch (restitutionCombineRule) {
            case MaterialCombineRule::Average:
                return (restitution + otherRestitution) * 0.5f;
            case MaterialCombineRule::Minimum:
                return restitution < otherRestitution ? restitution : otherRestitution;
            case MaterialCombineRule::Maximum:
                return restitution > otherRestitution ? restitution : otherRestitution;
            case MaterialCombineRule::Multiply:
                return restitution * otherRestitution;
            default:
                return restitution;
        }
    }

    /**
     * @brief Combine rolling resistance with another material
     */
    [[nodiscard]] float combineRollingResistance(float otherRollingResistance) const noexcept {
        // Rolling resistance typically uses average
        return (rollingResistance + otherRollingResistance) * 0.5f;
    }

    /**
     * @brief Get effective friction with another material
     */
    [[nodiscard]] float getEffectiveFriction(const Material& other) const noexcept {
        return combineFriction(other.friction);
    }

    /**
     * @brief Get effective restitution with another material
     */
    [[nodiscard]] float getEffectiveRestitution(const Material& other) const noexcept {
        return combineRestitution(other.restitution);
    }

    /**
     * @brief Clamp values to valid ranges
     */
    void clamp() noexcept {
        if (friction < 0.0f) friction = 0.0f;
        if (friction > 1.0f) friction = 1.0f;
        if (restitution < 0.0f) restitution = 0.0f;
        if (restitution > 1.0f) restitution = 1.0f;
        if (rollingResistance < 0.0f) rollingResistance = 0.0f;
        if (rollingResistance > 1.0f) rollingResistance = 1.0f;
        if (density <= 0.0f) density = 1.0f;
    }
    
    /**
     * @brief Validate material
     */
    [[nodiscard]] bool isValid() const noexcept {
        return friction >= 0.0f && friction <= 1.0f &&
               restitution >= 0.0f && restitution <= 1.0f &&
               density > 0.0f;
    }
};

} // namespace materials
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATERIALS_MATERIAL_H
