/**
 * @file capsule.h
 * @brief Capsule collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H

#include "../math/vector3.h"
#include "../math/matrix3x3.h"
#include "../math/bounds.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using math::Matrix3x3;
using math::AABB;

/**
 * @brief Capsule collision shape
 * 
 * Defined by center, radius, height, and orientation.
 * Used for:
 * - Character controllers
 * - Approximation of limbs
 * - Pill-shaped objects
 */
class Capsule {
public:
    Vector3 center;
    float radius;
    float height;
    Matrix3x3 orientation;
    
    /**
     * @brief Default constructor (Y-axis aligned)
     */
    constexpr Capsule() noexcept
        : center(0.0f, 0.0f, 0.0f)
        , radius(0.5f)
        , height(2.0f)
        , orientation(Matrix3x3::identity()) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Capsule(const Vector3& center_, float radius_, float height_) noexcept
        : center(center_)
        , radius(radius_)
        , height(height_)
        , orientation(Matrix3x3::identity()) {}
    
    /**
     * @brief Get AABB for capsule
     */
    [[nodiscard]] AABB getAABB() const noexcept {
        // Capsule is a cylinder + two hemispheres
        // Compute endpoints of the line segment
        Vector3 axis = orientation * Vector3(0.0f, height * 0.5f, 0.0f);
        Vector3 p1 = center - axis;
        Vector3 p2 = center + axis;
        
        // Expand AABB to include both spheres
        AABB result = AABB::fromSphere(p1, radius);
        result.expand(AABB::fromSphere(p2, radius));
        return result;
    }
    
    /**
     * @brief Get volume
     */
    [[nodiscard]] float getVolume() const noexcept {
        // Cylinder + two hemispheres = cylinder + sphere
        float cylinderVolume = 3.14159f * radius * radius * height;
        float sphereVolume = (4.0f / 3.0f) * 3.14159f * radius * radius * radius;
        return cylinderVolume + sphereVolume;
    }
    
    /**
     * @brief Get mass from density
     */
    [[nodiscard]] float getMass(float density) const noexcept {
        return getVolume() * density;
    }
    
    /**
     * @brief Get inertia tensor (capsule aligned with Y axis)
     */
    [[nodiscard]] Matrix3x3 getInertiaTensor(float mass) const noexcept {
        // Approximate as cylinder + sphere
        float cylinderMass = mass * 0.7f; // Approximate mass distribution
        float sphereMass = mass * 0.3f;
        
        // Cylinder inertia (aligned with Y)
        float Ixx_cyl = cylinderMass * (3.0f * radius * radius + height * height) / 12.0f;
        float Iyy_cyl = cylinderMass * radius * radius / 2.0f;
        float Izz_cyl = Ixx_cyl;
        
        // Sphere inertia
        float I_sphere = (2.0f / 5.0f) * sphereMass * radius * radius;
        
        // Total inertia (approximate)
        float Ixx = Ixx_cyl + I_sphere;
        float Iyy = Iyy_cyl + I_sphere;
        float Izz = Izz_cyl + I_sphere;
        
        return Matrix3x3::inertiaTensor(Ixx, Iyy, Izz);
    }
    
    /**
     * @brief Get capsule endpoints
     */
    void getEndpoints(Vector3& p1, Vector3& p2) const noexcept {
        Vector3 axis = orientation * Vector3(0.0f, height * 0.5f, 0.0f);
        p1 = center - axis;
        p2 = center + axis;
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H
