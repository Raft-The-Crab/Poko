/**
 * @file sphere.h
 * @brief Sphere collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H

#include "../math/vector3.h"
#include "../math/bounds.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using math::AABB;

/**
 * @brief Sphere collision shape
 * 
 * Simplest convex shape, defined by center and radius.
 * Used for:
 * - Fast collision detection
 * - Approximation of complex objects
 * - Particle systems
 * - Character controllers
 */
class Sphere {
public:
    Vector3 center;
    float radius;
    
    /**
     * @brief Default constructor
     */
    constexpr Sphere() noexcept : center(0.0f, 0.0f, 0.0f), radius(1.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Sphere(const Vector3& center_, float radius_) noexcept
        : center(center_), radius(radius_) {}
    
    /**
     * @brief Get AABB for sphere
     */
    [[nodiscard]] AABB getAABB() const noexcept {
        return AABB::fromSphere(center, radius);
    }
    
    /**
     * @brief Get volume
     */
    [[nodiscard]] float getVolume() const noexcept {
        return (4.0f / 3.0f) * 3.14159f * radius * radius * radius;
    }
    
    /**
     * @brief Get mass from density
     */
    [[nodiscard]] float getMass(float density) const noexcept {
        return getVolume() * density;
    }
    
    /**
     * @brief Get inertia tensor (sphere is uniform)
     */
    [[nodiscard]] float getInertia(float mass) const noexcept {
        return (2.0f / 5.0f) * mass * radius * radius;
    }
    
    /**
     * @brief Check if point is inside sphere
     */
    [[nodiscard]] bool contains(const Vector3& point) const noexcept {
        return center.distanceSquaredTo(point) <= radius * radius;
    }
    
    /**
     * @brief Sphere-sphere intersection test
     */
    [[nodiscard]] bool intersects(const Sphere& other) const noexcept {
        float distSq = center.distanceSquaredTo(other.center);
        float radiusSum = radius + other.radius;
        return distSq <= radiusSum * radiusSum;
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H
