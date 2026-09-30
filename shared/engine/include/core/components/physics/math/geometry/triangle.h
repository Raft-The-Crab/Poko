/**
 * @file triangle.h
 * @brief Triangle for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_TRIANGLE_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_TRIANGLE_H

#include "vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Triangle
 */
class Triangle {
public:
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;
    
    /**
     * @brief Default constructor
     */
    constexpr Triangle() noexcept
        : v0(0.0f, 0.0f, 0.0f)
        , v1(1.0f, 0.0f, 0.0f)
        , v2(0.0f, 1.0f, 0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Triangle(const Vector3& v0_, const Vector3& v1_, const Vector3& v2_) noexcept
        : v0(v0_)
        , v1(v1_)
        , v2(v2_) {}
    
    /**
     * @brief Get normal
     */
    [[nodiscard]] Vector3 normal() const noexcept {
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        return edge1.cross(edge2).normalized();
    }
    
    /**
     * @brief Get area
     */
    [[nodiscard]] float area() const noexcept {
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        return edge1.cross(edge2).length() * 0.5f;
    }
    
    /**
     * @brief Get centroid
     */
    [[nodiscard]] Vector3 centroid() const noexcept {
        return (v0 + v1 + v2) / 3.0f;
    }
    
    /**
     * @brief Check if point is inside triangle (barycentric)
     */
    [[nodiscard]] bool contains(const Vector3& point) const noexcept {
        Vector3 v0v1 = v1 - v0;
        Vector3 v0v2 = v2 - v0;
        Vector3 v0p = point - v0;
        
        float d00 = v0v1.dot(v0v1);
        float d01 = v0v1.dot(v0v2);
        float d11 = v0v2.dot(v0v2);
        float d20 = v0p.dot(v0v1);
        float d21 = v0p.dot(v0v2);
        
        float denom = d00 * d11 - d01 * d01;
        if (std::abs(denom) < 0.0001f) return false;
        
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;
        
        return u >= 0.0f && v >= 0.0f && w >= 0.0f;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_TRIANGLE_H
