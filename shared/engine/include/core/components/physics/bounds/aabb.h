/**
 * @file aabb.h
 * @brief Axis-aligned bounding box
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_BOUNDS_AABB_H
#define POKO_CORE_COMPONENTS_PHYSICS_BOUNDS_AABB_H

#include "core/components/physics/math/vectors/vector3.h"
#include <cmath>
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace bounds {

using math::Vector3;

/**
 * @brief Axis-aligned bounding box
 */
class AABB {
public:
    Vector3 min;
    Vector3 max;
    
    constexpr AABB() noexcept
        : min(0.0f, 0.0f, 0.0f)
        , max(0.0f, 0.0f, 0.0f) {}
    
    constexpr AABB(const Vector3& min_, const Vector3& max_) noexcept
        : min(min_)
        , max(max_) {}
    
    static AABB fromCenterExtent(const Vector3& center, const Vector3& extent) noexcept {
        return AABB{center - extent, center + extent};
    }
    
    [[nodiscard]] Vector3 center() const noexcept {
        return (min + max) * 0.5f;
    }
    
    [[nodiscard]] Vector3 extent() const noexcept {
        return (max - min) * 0.5f;
    }
    
    [[nodiscard]] Vector3 size() const noexcept {
        return max - min;
    }
    
    [[nodiscard]] float volume() const noexcept {
        Vector3 s = size();
        return s.x * s.y * s.z;
    }
    
    [[nodiscard]] bool isValid() const noexcept {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }
    
    [[nodiscard]] bool isEmpty() const noexcept {
        return min.x >= max.x || min.y >= max.y || min.z >= max.z;
    }
    
    void expandToInclude(const Vector3& point) noexcept {
        min = min.min(point);
        max = max.max(point);
    }
    
    void expandToInclude(const AABB& other) noexcept {
        min = min.min(other.min);
        max = max.max(other.max);
    }
    
    [[nodiscard]] bool contains(const Vector3& point) const noexcept {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
    
    [[nodiscard]] bool intersects(const AABB& other) const noexcept {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    /**
     * @brief Get surface area
     */
    [[nodiscard]] float area() const noexcept {
        Vector3 s = size();
        return 2.0f * (s.x * s.y + s.y * s.z + s.z * s.x);
    }
    
    /**
     * @brief Check if AABB contains min point
     */
    [[nodiscard]] bool containsMin(const Vector3& point) const noexcept {
        return point.x >= min.x && point.y >= min.y && point.z >= min.z;
    }

    /**
     * @brief Check if AABB contains max point
     */
    [[nodiscard]] bool containsMax(const Vector3& point) const noexcept {
        return point.x <= max.x && point.y <= max.y && point.z <= max.z;
    }

    /**
     * @brief Check if AABB contains another AABB
     */
    [[nodiscard]] bool contains(const AABB& other) const noexcept {
        return containsMin(other.min) && containsMax(other.max);
    }

    /**
     * @brief Ray-AABB intersection (Slab method)
     */
    [[nodiscard]] bool intersectsRay(const Vector3& origin, const Vector3& direction, float& tMin, float& tMax) const noexcept {
        tMin = 0.0f;
        tMax = 1e30f;

        for (int i = 0; i < 3; ++i) {
            if (std::abs(direction.getComponent(i)) < 0.0001f) {
                // Ray is parallel to slab
                if (origin.getComponent(i) < min.getComponent(i) || origin.getComponent(i) > max.getComponent(i)) {
                    return false;
                }
            } else {
                float invDir = 1.0f / direction.getComponent(i);
                float t1 = (min.getComponent(i) - origin.getComponent(i)) * invDir;
                float t2 = (max.getComponent(i) - origin.getComponent(i)) * invDir;

                if (invDir < 0.0f) {
                    // Swap
                    float temp = t1;
                    t1 = t2;
                    t2 = temp;
                }

                tMin = t1 > tMin ? t1 : tMin;
                tMax = t2 < tMax ? t2 : tMax;

                if (tMax < tMin) {
                    return false;
                }
            }
        }

        return tMin <= tMax;
    }

    /**
     * @brief Translate AABB
     */
    [[nodiscard]] AABB translated(const Vector3& offset) const noexcept {
        return AABB{min + offset, max + offset};
    }

    /**
     * @brief Scale AABB
     */
    [[nodiscard]] AABB scaled(float scale) const noexcept {
        Vector3 center = this->center();
        Vector3 extent = this->extent() * scale;
        return fromCenterExtent(center, extent);
    }

    /**
     * @brief Union of two AABBs
     */
    [[nodiscard]] AABB unionWith(const AABB& other) const noexcept {
        AABB result;
        result.min.x = min.x < other.min.x ? min.x : other.min.x;
        result.min.y = min.y < other.min.y ? min.y : other.min.y;
        result.min.z = min.z < other.min.z ? min.z : other.min.z;
        result.max.x = max.x > other.max.x ? max.x : other.max.x;
        result.max.y = max.y > other.max.y ? max.y : other.max.y;
        result.max.z = max.z > other.max.z ? max.z : other.max.z;
        return result;
    }

    /**
     * @brief Intersection of two AABBs
     */
    [[nodiscard]] AABB intersectionWith(const AABB& other) const noexcept {
        AABB result;
        result.min.x = min.x > other.min.x ? min.x : other.min.x;
        result.min.y = min.y > other.min.y ? min.y : other.min.y;
        result.min.z = min.z > other.min.z ? min.z : other.min.z;
        result.max.x = max.x < other.max.x ? max.x : other.max.x;
        result.max.y = max.y < other.max.y ? max.y : other.max.y;
        result.max.z = max.z < other.max.z ? max.z : other.max.z;
        return result;
    }

    /**
     * @brief Make fat AABB for broadphase
     */
    [[nodiscard]] AABB makeFat(float margin) const noexcept {
        Vector3 offset = Vector3(margin, margin, margin);
        return AABB{min - offset, max + offset};
    }
};

} // namespace bounds
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BOUNDS_AABB_H
