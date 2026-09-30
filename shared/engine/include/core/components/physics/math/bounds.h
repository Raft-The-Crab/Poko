/**
 * @file bounds.h
 * @brief Axis-aligned bounding box (AABB) for collision detection
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_BOUNDS_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_BOUNDS_H

#include "vector3.h"
#include <algorithm>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Axis-aligned bounding box (AABB)
 * 
 * Used for:
 * - Broadphase collision detection (spatial partitioning)
 * - Shape bounds calculations
 * - Frustum culling
 * - Spatial queries
 * 
 * Stored as min and max corners:
 * min: (minX, minY, minZ) - minimum extent
 * max: (maxX, maxY, maxZ) - maximum extent
 * 
 * @section performance Performance
 * AABBs are extremely fast for:
 * - Overlap testing (6 comparisons)
 * - Ray intersection (Slab method)
 * - Point containment (6 comparisons)
 */
class AABB {
public:
    Vector3 min;  // Minimum corner
    Vector3 max;  // Maximum corner
    
    // ============================================================================
    // Constructors
    // ============================================================================
    
    /**
     * @brief Default constructor (invalid bounds)
     */
    constexpr AABB() noexcept : min(1e30f, 1e30f, 1e30f), max(-1e30f, -1e30f, -1e30f) {}
    
    /**
     * @brief Min-max constructor
     */
    constexpr AABB(const Vector3& min_, const Vector3& max_) noexcept : min(min_), max(max_) {}
    
    /**
     * @brief Center-extent constructor
     */
    static AABB fromCenterExtent(const Vector3& center, const Vector3& extent) noexcept {
        return AABB(center - extent, center + extent);
    }
    
    /**
     * @brief Create AABB from sphere
     */
    static AABB fromSphere(const Vector3& center, float radius) noexcept {
        Vector3 extent(radius, radius, radius);
        return fromCenterExtent(center, extent);
    }
    
    /**
     * @brief Create AABB from box (oriented, then axis-aligned)
     */
    static AABB fromBox(const Vector3& center, const Vector3& halfExtents) noexcept {
        return fromCenterExtent(center, halfExtents);
    }
    
    // ============================================================================
    // AABB Properties
    // ============================================================================
    
    /**
     * @brief Get center of AABB
     */
    [[nodiscard]] Vector3 getCenter() const noexcept {
        return Vector3(
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f,
            (min.z + max.z) * 0.5f
        );
    }
    
    /**
     * @brief Get extent (half-size) of AABB
     */
    [[nodiscard]] Vector3 getExtent() const noexcept {
        return Vector3(
            (max.x - min.x) * 0.5f,
            (max.y - min.y) * 0.5f,
            (max.z - min.z) * 0.5f
        );
    }
    
    /**
     * @brief Get size (full size) of AABB
     */
    [[nodiscard]] Vector3 getSize() const noexcept {
        return max - min;
    }
    
    /**
     * @brief Get volume of AABB
     */
    [[nodiscard]] float getVolume() const noexcept {
        Vector3 size = getSize();
        return size.x * size.y * size.z;
    }
    
    /**
     * @brief Get surface area of AABB
     */
    [[nodiscard]] float getSurfaceArea() const noexcept {
        Vector3 size = getSize();
        return 2.0f * (size.x * size.y + size.y * size.z + size.z * size.x);
    }
    
    /**
     * @brief Check if AABB is valid (min <= max)
     */
    [[nodiscard]] bool isValid() const noexcept {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }
    
    /**
     * @brief Check if AABB is empty (no volume)
     */
    [[nodiscard]] bool isEmpty() const noexcept {
        return min.x >= max.x || min.y >= max.y || min.z >= max.z;
    }
    
    // ============================================================================
    // AABB Operations
    // ============================================================================
    
    /**
     * @brief Expand AABB to include a point
     */
    void expand(const Vector3& point) noexcept {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }
    
    /**
     * @brief Expand AABB to include another AABB
     */
    void expand(const AABB& other) noexcept {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        min.z = std::min(min.z, other.min.z);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
        max.z = std::max(max.z, other.max.z);
    }
    
    /**
     * @brief Expand AABB by a margin (for fat AABBs in broadphase)
     */
    void expand(float margin) noexcept {
        min.x -= margin;
        min.y -= margin;
        min.z -= margin;
        max.x += margin;
        max.y += margin;
        max.z += margin;
    }
    
    /**
     * @brief Translate AABB
     */
    [[nodiscard]] AABB translated(const Vector3& offset) const noexcept {
        return AABB(min + offset, max + offset);
    }
    
    /**
     * @brief Translate AABB in place
     */
    void translate(const Vector3& offset) noexcept {
        min = min + offset;
        max = max + offset;
    }
    
    /**
     * @brief Scale AABB
     */
    [[nodiscard]] AABB scaled(const Vector3& scale) const noexcept {
        Vector3 center = getCenter();
        Vector3 extent = getExtent();
        extent.x *= scale.x;
        extent.y *= scale.y;
        extent.z *= scale.z;
        return fromCenterExtent(center, extent);
    }
    
    // ============================================================================
    // Collision Tests
    // ============================================================================
    
    /**
     * @brief Check if point is inside AABB
     */
    [[nodiscard]] bool contains(const Vector3& point) const noexcept {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
    
    /**
     * @brief Check if AABB intersects another AABB
     */
    [[nodiscard]] bool intersects(const AABB& other) const noexcept {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    /**
     * @brief Check if AABB fully contains another AABB
     */
    [[nodiscard]] bool contains(const AABB& other) const noexcept {
        return min.x <= other.min.x && max.x >= other.max.x &&
               min.y <= other.min.y && max.y >= other.max.y &&
               min.z <= other.min.z && max.z >= other.max.z;
    }
    
    /**
     * @brief Ray-AABB intersection (Slab method)
     * @param origin Ray origin
     * @param direction Ray direction (must be normalized)
     * @param tMin Output: minimum intersection distance
     * @param tMax Output: maximum intersection distance
     * @return True if ray intersects AABB
     */
    [[nodiscard]] bool rayIntersect(
        const Vector3& origin,
        const Vector3& direction,
        float& tMin,
        float& tMax
    ) const noexcept {
        tMin = 0.0f;
        tMax = 1e30f;
        
        for (int i = 0; i < 3; ++i) {
            float minVal = (&min.x)[i];
            float maxVal = (&max.x)[i];
            float originVal = (&origin.x)[i];
            float dirVal = (&direction.x)[i];
            
            if (std::abs(dirVal) < 0.0001f) {
                // Ray is parallel to slab
                if (originVal < minVal || originVal > maxVal) {
                    return false;
                }
            } else {
                float t1 = (minVal - originVal) / dirVal;
                float t2 = (maxVal - originVal) / dirVal;
                
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                
                if (tMin > tMax) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    /**
     * @brief Union of two AABBs
     */
    [[nodiscard]] static AABB unionOf(const AABB& a, const AABB& b) noexcept {
        AABB result = a;
        result.expand(b);
        return result;
    }
    
    /**
     * @brief Intersection of two AABBs
     */
    [[nodiscard]] static AABB intersectionOf(const AABB& a, const AABB& b) noexcept {
        return AABB(
            Vector3(
                std::max(a.min.x, b.min.x),
                std::max(a.min.y, b.min.y),
                std::max(a.min.z, b.min.z)
            ),
            Vector3(
                std::min(a.max.x, b.max.x),
                std::min(a.max.y, b.max.y),
                std::min(a.max.z, b.max.z)
            )
        );
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_BOUNDS_H
