/**
 * @file triangle_mesh.h
 * @brief Triangle mesh collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_TRIANGLE_MESH_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_TRIANGLE_MESH_H

#include "../math/vector3.h"
#include "../math/bounds.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using math::AABB;

/**
 * @brief Triangle index
 */
struct Triangle {
    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
    
    Triangle() noexcept : i0(0), i1(0), i2(0) {}
    Triangle(uint32_t i0_, uint32_t i1_, uint32_t i2_) noexcept
        : i0(i0_), i1(i1_), i2(i2_) {}
};

/**
 * @brief Triangle mesh collision shape
 * 
 * Represents a triangle mesh for static geometry.
 * Used for terrain, buildings, and complex static objects.
 */
class TriangleMesh {
public:
    /**
     * @brief Default constructor
     */
    TriangleMesh() noexcept
        : vertices()
        , triangles()
        , margin(0.0f) {}
    
    /**
     * @brief Construct from vertices and triangles
     */
    TriangleMesh(const std::vector<Vector3>& vertices_,
                 const std::vector<Triangle>& triangles_) noexcept
        : vertices(vertices_)
        , triangles(triangles_)
        , margin(0.0f) {
        computeAABB();
    }
    
    /**
     * @brief Get vertices
     */
    [[nodiscard]] const std::vector<Vector3>& getVertices() const noexcept {
        return vertices;
    }
    
    /**
     * @brief Get triangles
     */
    [[nodiscard]] const std::vector<Triangle>& getTriangles() const noexcept {
        return triangles;
    }
    
    /**
     * @brief Get vertex count
     */
    [[nodiscard]] uint32_t getVertexCount() const noexcept {
        return static_cast<uint32_t>(vertices.size());
    }
    
    /**
     * @brief Get triangle count
     */
    [[nodiscard]] uint32_t getTriangleCount() const noexcept {
        return static_cast<uint32_t>(triangles.size());
    }
    
    /**
     * @brief Get AABB
     */
    [[nodiscard]] AABB getAABB() const noexcept {
        return aabb;
    }
    
    /**
     * @brief Get margin
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
     * @brief Get triangle vertices
     */
    [[nodiscard]] void getTriangleVertices(uint32_t triangleIndex,
                                            Vector3& v0,
                                            Vector3& v1,
                                            Vector3& v2) const noexcept {
        if (triangleIndex >= triangles.size()) {
            v0 = Vector3::zero();
            v1 = Vector3::zero();
            v2 = Vector3::zero();
            return;
        }
        
        const Triangle& tri = triangles[triangleIndex];
        v0 = vertices[tri.i0];
        v1 = vertices[tri.i1];
        v2 = vertices[tri.i2];
    }
    
    /**
     * @brief Ray-triangle intersection test
     * Uses Möller-Trumbore algorithm
     */
    [[nodiscard]] bool rayIntersect(uint32_t triangleIndex,
                                     const Vector3& origin,
                                     const Vector3& direction,
                                     float& t) const noexcept {
        Vector3 v0, v1, v2;
        getTriangleVertices(triangleIndex, v0, v1, v2);
        
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        
        Vector3 h = direction.cross(edge2);
        float a = edge1.dot(h);
        
        if (std::abs(a) < 0.0001f) {
            return false; // Ray is parallel to triangle
        }
        
        float f = 1.0f / a;
        Vector3 s = origin - v0;
        float u = f * s.dot(h);
        
        if (u < 0.0f || u > 1.0f) {
            return false;
        }
        
        Vector3 q = s.cross(edge1);
        float v = f * direction.dot(q);
        
        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }
        
        t = f * edge2.dot(q);
        
        return t > 0.0f;
    }
    
    /**
     * @brief Compute triangle normal
     */
    [[nodiscard]] Vector3 getTriangleNormal(uint32_t triangleIndex) const noexcept {
        Vector3 v0, v1, v2;
        getTriangleVertices(triangleIndex, v0, v1, v2);
        
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        
        return edge1.cross(edge2).normalized();
    }
    
private:
    /**
     * @brief Compute AABB from vertices
     */
    void computeAABB() noexcept {
        if (vertices.empty()) {
            aabb = AABB::fromCenterExtent(Vector3::zero(), Vector3::zero());
            return;
        }
        
        aabb = AABB();
        for (const Vector3& v : vertices) {
            aabb.expand(v);
        }
    }
    
    std::vector<Vector3> vertices;
    std::vector<Triangle> triangles;
    AABB aabb;
    float margin;
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_TRIANGLE_MESH_H
