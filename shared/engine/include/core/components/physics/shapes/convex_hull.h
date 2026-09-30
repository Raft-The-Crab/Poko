/**
 * @file convex_hull.h
 * @brief Convex hull collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CONVEX_HULL_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CONVEX_HULL_H

#include "../math/vector3.h"
#include "../math/matrix3x3.h"
#include "../math/bounds.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using math::Matrix3x3;
using math::AABB;

/**
 * @brief Convex hull collision shape
 * 
 * Represents a convex polyhedron defined by vertices.
 * Used for complex convex shapes in physics simulation.
 */
class ConvexHull {
public:
    /**
     * @brief Default constructor
     */
    ConvexHull() noexcept
        : vertices()
        , center(0.0f, 0.0f, 0.0f)
        , margin(0.0f) {}
    
    /**
     * @brief Construct from vertices
     */
    explicit ConvexHull(const std::vector<Vector3>& vertices_) noexcept
        : vertices(vertices_)
        , margin(0.0f) {
        computeCenter();
    }
    
    /**
     * @brief Construct from vertices with margin
     */
    ConvexHull(const std::vector<Vector3>& vertices_, float margin_) noexcept
        : vertices(vertices_)
        , margin(margin_) {
        computeCenter();
    }
    
    /**
     * @brief Get vertices
     */
    [[nodiscard]] const std::vector<Vector3>& getVertices() const noexcept {
        return vertices;
    }
    
    /**
     * @brief Get vertex count
     */
    [[nodiscard]] uint32_t getVertexCount() const noexcept {
        return static_cast<uint32_t>(vertices.size());
    }
    
    /**
     * @brief Get center
     */
    [[nodiscard]] Vector3 getCenter() const noexcept {
        return center;
    }
    
    /**
     * @brief Get margin (collision margin)
     */
    [[nodiscard]] float getMargin() const noexcept {
        return margin;
    }
    
    /**
     * @brief Set margin
     */
    void setMargin(float margin_) noexcept {
        margin = margin_;
    }
    
    /**
     * @brief Compute AABB
     */
    [[nodiscard]] AABB getAABB() const noexcept {
        if (vertices.empty()) {
            return AABB::fromCenterExtent(Vector3::zero(), Vector3::zero());
        }
        
        AABB result;
        for (const Vector3& v : vertices) {
            result.expand(v);
        }
        
        // Expand by margin
        if (margin > 0.0f) {
            result = result.fat(margin);
        }
        
        return result;
    }
    
    /**
     * @brief Compute volume
     * Uses tetrahedron decomposition method
     */
    [[nodiscard]] float getVolume() const noexcept {
        if (vertices.size() < 4) return 0.0f;
        
        // Simplified: treat as bounding box for now
        AABB aabb = getAABB();
        Vector3 size = aabb.getSize();
        return size.x * size.y * size.z;
    }
    
    /**
     * @brief Compute mass from density
     */
    [[nodiscard]] float computeMass(float density) const noexcept {
        return getVolume() * density;
    }
    
    /**
     * @brief Compute inertia tensor (approximate)
     * Uses bounding box approximation
     */
    [[nodiscard]] Matrix3x3 computeInertia(float mass) const noexcept {
        AABB aabb = getAABB();
        Vector3 size = aabb.getSize();
        
        float hx = size.x * 0.5f;
        float hy = size.y * 0.5f;
        float hz = size.z * 0.5f;
        
        float ixx = mass / 12.0f * (hy * hy + hz * hz);
        float iyy = mass / 12.0f * (hx * hx + hz * hz);
        float izz = mass / 12.0f * (hx * hx + hy * hy);
        
        return Matrix3x3::diagonal(ixx, iyy, izz);
    }
    
    /**
     * @brief Support function for GJK
     * Returns furthest point in given direction
     */
    [[nodiscard]] Vector3 support(const Vector3& direction) const noexcept {
        if (vertices.empty()) return center;
        
        float maxDot = vertices[0].dot(direction);
        Vector3 result = vertices[0];
        
        for (const Vector3& v : vertices) {
            float dot = v.dot(direction);
            if (dot > maxDot) {
                maxDot = dot;
                result = v;
            }
        }
        
        return result;
    }
    
    /**
     * @brief Check if point is inside hull
     */
    [[nodiscard]] bool contains(const Vector3& point) const noexcept {
        // Simplified: check against AABB
        AABB aabb = getAABB();
        return aabb.contains(point);
    }
    
private:
    /**
     * @brief Compute center of mass
     */
    void computeCenter() noexcept {
        if (vertices.empty()) {
            center = Vector3::zero();
            return;
        }
        
        Vector3 sum = Vector3::zero();
        for (const Vector3& v : vertices) {
            sum = sum + v;
        }
        
        center = sum / static_cast<float>(vertices.size());
    }
    
    std::vector<Vector3> vertices;
    Vector3 center;
    float margin;
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CONVEX_HULL_H
