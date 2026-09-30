/**
 * @file material.cpp
 * @brief Physics material implementation
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/materials/material.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace materials {

Material::Material(float friction_, float restitution_, float density_) noexcept
    : handle()
    , friction(friction_)
    , restitution(restitution_)
    , rollingResistance(0.0f)
    , density(density_)
    , frictionCombineRule(MaterialCombineRule::Multiply)
    , restitutionCombineRule(MaterialCombineRule::Maximum) {
    // Clamp values to valid ranges
    if (friction < 0.0f) friction = 0.0f;
    if (friction > 1.0f) friction = 1.0f;
    if (restitution < 0.0f) restitution = 0.0f;
    if (restitution > 1.0f) restitution = 1.0f;
    if (density <= 0.0f) density = 1.0f;
}

} // namespace materials
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
