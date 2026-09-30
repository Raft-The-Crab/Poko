/**
 * @file calculate_inertia.cpp
 * @brief Calculate inertia tensor from mass and shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/physics.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {

// Production inertia calculation constants
constexpr float PI = 3.14159265358979323846f;

void Physics::calculateInertiaTensor() noexcept {
    // Reset to identity
    m_inertiaXX = 1.0f;
    m_inertiaXY = 0.0f;
    m_inertiaXZ = 0.0f;
    m_inertiaYX = 0.0f;
    m_inertiaYY = 1.0f;
    m_inertiaYZ = 0.0f;
    m_inertiaZX = 0.0f;
    m_inertiaZY = 0.0f;
    m_inertiaZZ = 1.0f;
    
    float mass = m_mass;
    if (mass <= 0.0f) mass = 1.0f;
    
    switch (m_shapeType) {
        case ShapeType::Sphere:
            // Sphere inertia: I = (2/5) * m * r² (diagonal, all axes equal)
            {
                float i = (2.0f / 5.0f) * mass * m_sphereRadius * m_sphereRadius;
                m_inertiaXX = i;
                m_inertiaYY = i;
                m_inertiaZZ = i;
            }
            break;
            
        case ShapeType::Box:
            // Box inertia (with half-extents):
            // Ixx = m/12 * (hy² + hz²)
            // Iyy = m/12 * (hx² + hz²)
            // Izz = m/12 * (hx² + hy²)
            {
                float factor = mass / 12.0f;
                float hx2 = m_boxHalfExtX * m_boxHalfExtX;
                float hy2 = m_boxHalfExtY * m_boxHalfExtY;
                float hz2 = m_boxHalfExtZ * m_boxHalfExtZ;
                
                m_inertiaXX = factor * (hy2 + hz2);
                m_inertiaYY = factor * (hx2 + hz2);
                m_inertiaZZ = factor * (hx2 + hy2);
            }
            break;
            
        case ShapeType::Capsule:
            // Capsule inertia (cylinder + hemispheres) aligned along Y axis
            {
                float r = m_capsuleRadius;
                float h = m_capsuleHeight;
                float cylinderHeight = h - 2.0f * r;
                if (cylinderHeight < 0.0f) cylinderHeight = 0.0f;
                
                // Cylinder mass proportion
                float cylinderVolume = PI * r * r * cylinderHeight;
                float hemisphereVolume = (4.0f / 3.0f) * PI * r * r * r;
                float totalVolume = cylinderVolume + hemisphereVolume;
                
                if (totalVolume > 0.0f) {
                    float cylinderMass = mass * (cylinderVolume / totalVolume);
                    float hemisphereMass = mass * (hemisphereVolume / totalVolume);
                    
                    // Cylinder inertia (along Y axis)
                    // Ixx = Izz = m * (3r² + h²) / 12
                    float cylinderInertia = cylinderMass * (3.0f * r * r + cylinderHeight * cylinderHeight) / 12.0f;
                    
                    // Hemisphere inertia (approximate as point mass at center)
                    // Ixx = Izz = (2/5) * m * r² + m * d² (parallel axis theorem)
                    float hemisphereInertia = (2.0f / 5.0f) * hemisphereMass * r * r;
                    
                    m_inertiaXX = cylinderInertia + hemisphereInertia;
                    m_inertiaYY = cylinderMass * r * r / 2.0f + hemisphereMass * (2.0f / 5.0f) * r * r;
                    m_inertiaZZ = cylinderInertia + hemisphereInertia;
                }
            }
            break;
            
        case ShapeType::Cylinder:
            // Cylinder inertia (along Y axis)
            // Ixx = Izz = m * (3r² + h²) / 12
            // Iyy = m * r² / 2
            {
                float r = m_capsuleRadius;
                float h = m_capsuleHeight;
                float radialInertia = mass * (3.0f * r * r + h * h) / 12.0f;
                float axialInertia = mass * r * r / 2.0f;
                
                m_inertiaXX = radialInertia;
                m_inertiaYY = axialInertia;
                m_inertiaZZ = radialInertia;
            }
            break;
            
        case ShapeType::ConvexHull:
        case ShapeType::TriangleMesh:
        case ShapeType::Heightfield:
            // Complex shapes - default to identity
            // In production, would compute from vertices
            break;
    }
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
