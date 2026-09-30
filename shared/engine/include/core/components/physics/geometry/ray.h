/**
 * @file ray.h
 * @brief Ray for physics queries
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_GEOMETRY_RAY_H
#define POKO_CORE_COMPONENTS_PHYSICS_GEOMETRY_RAY_H

#include "../math/vectors/vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace geometry {

using math::Vector3;

/**
 * @brief Ray for physics queries
 */
class Ray {
public:
    Vector3 origin;
    Vector3 direction;
    
    constexpr Ray() noexcept
        : origin(0.0f, 0.0f, 0.0f)
        , direction(0.0f, 0.0f, 1.0f) {}
    
    constexpr Ray(const Vector3& origin_, const Vector3& direction_) noexcept
        : origin(origin_)
        , direction(direction_) {}
    
    [[nodiscard]] Vector3 pointAt(float t) const noexcept {
        return origin + direction * t;
    }
};

} // namespace geometry
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_GEOMETRY_RAY_H
