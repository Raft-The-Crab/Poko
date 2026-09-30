/**
 * @file line_segment.h
 * @brief Line segment for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_LINE_SEGMENT_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_LINE_SEGMENT_H

#include "vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Line segment
 */
class LineSegment {
public:
    Vector3 p0;
    Vector3 p1;
    
    /**
     * @brief Default constructor
     */
    constexpr LineSegment() noexcept
        : p0(0.0f, 0.0f, 0.0f)
        , p1(1.0f, 0.0f, 0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr LineSegment(const Vector3& p0_, const Vector3& p1_) noexcept
        : p0(p0_)
        , p1(p1_) {}
    
    /**
     * @brief Get direction
     */
    [[nodiscard]] Vector3 direction() const noexcept {
        return (p1 - p0).normalized();
    }
    
    /**
     * @brief Get length
     */
    [[nodiscard]] float length() const noexcept {
        return p0.distanceTo(p1);
    }
    
    /**
     * @brief Get length squared
     */
    [[nodiscard]] float lengthSquared() const noexcept {
        return p0.distanceSquaredTo(p1);
    }
    
    /**
     * @brief Get point at parameter t (0 to 1)
     */
    [[nodiscard]] Vector3 pointAt(float t) const noexcept {
        return Vector3::lerp(p0, p1, t);
    }
    
    /**
     * @brief Closest point on segment to a point
     */
    [[nodiscard]] Vector3 closestPoint(const Vector3& point) const noexcept {
        Vector3 seg = p1 - p0;
        float lenSq = seg.lengthSquared();
        
        if (lenSq < 0.0001f) {
            return p0;
        }
        
        float t = (point - p0).dot(seg) / lenSq;
        t = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
        
        return p0 + seg * t;
    }
    
    /**
     * @brief Distance from point to segment
     */
    [[nodiscard]] float distanceTo(const Vector3& point) const noexcept {
        return point.distanceTo(closestPoint(point));
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_LINE_SEGMENT_H
