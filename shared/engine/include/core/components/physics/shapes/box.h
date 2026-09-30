/**
 * @file box.h
 * @brief Box (OBB) collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H

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
 * @brief Oriented bounding box (OBB) collision shape
 * 
 * Defined by center, half-extents, and orientation.
 * Used for:
 * - Static geometry
 * - Dynamic objects
 * - Collision detection with SAT
 */
class Box {
public:
    Vector3 center;
    Vector3 halfExtents;
    Matrix3x3 orientation;
    
    /**
     * @brief Default constructor (axis-aligned)
     */
    constexpr Box() noexcept
        : center(0.0f, 0.0f, 0.0f)
        , halfExtents(1.0f, 1.0f, 1.0f)
        , orientation(Matrix3x3::identity()) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Box(const Vector3& center_, const Vector3& halfExtents_) noexcept
        : center(center_)
        , halfExtents(halfExtents_)
        , orientation(Matrix3x3::identity()) {}
    
    /**
     * @brief Oriented constructor
     */
    Box(const Vector3& center_, const Vector3& halfExtents_, const Matrix3x3& orientation_) noexcept
        : center(center_)
        , halfExtents(halfExtents_)
        , orientation(orientation_) {}
    
    /**
     * @brief Get AABB for box (loose bound)
     */
    [[nodiscard]] AABB getAABB() const noexcept {
        // For axis-aligned boxes, this is exact
        if (orientation.isIdentity()) {
            return AABB::fromCenterExtent(center, halfExtents);
        }
        
        // For oriented boxes, compute projected extents
        Vector3 corners[8];
        getCorners(corners);
        
        AABB result;
        for (int i = 0; i < 8; ++i) {
            result.expand(corners[i]);
        }
        return result;
    }
    
    /**
     * @brief Get volume
     */
    [[nodiscard]] float getVolume() const noexcept {
        return 8.0f * halfExtents.x * halfExtents.y * halfExtents.z;
    }
    
    /**
     * @brief Get mass from density
     */
    [[nodiscard]] float getMass(float density) const noexcept {
        return getVolume() * density;
    }
    
    /**
     * @brief Get inertia tensor (box)
     */
    [[nodiscard]] Matrix3x3 getInertiaTensor(float mass) const noexcept {
        float hx = halfExtents.x;
        float hy = halfExtents.y;
        float hz = halfExtents.z;
        
        float Ixx = (mass / 12.0f) * (hy * hy + hz * hz);
        float Iyy = (mass / 12.0f) * (hx * hx + hz * hz);
        float Izz = (mass / 12.0f) * (hx * hx + hy * hy);
        
        return Matrix3x3::inertiaTensor(Ixx, Iyy, Izz);
    }
    
    /**
     * @brief Get the 8 corners of the box
     */
    void getCorners(Vector3 corners[8]) const noexcept {
        Vector3 axes[3];
        axes[0] = orientation * Vector3(halfExtents.x, 0.0f, 0.0f);
        axes[1] = orientation * Vector3(0.0f, halfExtents.y, 0.0f);
        axes[2] = orientation * Vector3(0.0f, 0.0f, halfExtents.z);
        
        corners[0] = center - axes[0] - axes[1] - axes[2];
        corners[1] = center + axes[0] - axes[1] - axes[2];
        corners[2] = center + axes[0] + axes[1] - axes[2];
        corners[3] = center - axes[0] + axes[1] - axes[2];
        corners[4] = center - axes[0] - axes[1] + axes[2];
        corners[5] = center + axes[0] - axes[1] + axes[2];
        corners[6] = center + axes[0] + axes[1] + axes[2];
        corners[7] = center - axes[0] + axes[1] + axes[2];
    }
    
    /**
     * @brief Get face normals
     */
    void getNormals(Vector3 normals[6]) const noexcept {
        normals[0] = orientation * Vector3(1.0f, 0.0f, 0.0f);
        normals[1] = orientation * Vector3(-1.0f, 0.0f, 0.0f);
        normals[2] = orientation * Vector3(0.0f, 1.0f, 0.0f);
        normals[3] = orientation * Vector3(0.0f, -1.0f, 0.0f);
        normals[4] = orientation * Vector3(0.0f, 0.0f, 1.0f);
        normals[5] = orientation * Vector3(0.0f, 0.0f, -1.0f);
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H
