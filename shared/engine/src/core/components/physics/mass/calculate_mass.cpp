/**
 * @file calculate_mass.cpp
 * @brief Calculate mass from density and shape
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

// Production mass calculation constants
constexpr float PI = 3.14159265358979323846f;

float Physics::calculateMassFromDensity() const noexcept {
    // Calculate volume based on shape type
    float volume = 0.0f;
    
    switch (m_shapeType) {
        case ShapeType::Sphere:
            // Volume = (4/3) * π * r³
            volume = (4.0f / 3.0f) * PI * m_sphereRadius * m_sphereRadius * m_sphereRadius;
            break;
            
        case ShapeType::Box:
            // Volume = (2*halfExtX) * (2*halfExtY) * (2*halfExtZ)
            volume = (2.0f * m_boxHalfExtX) * (2.0f * m_boxHalfExtY) * (2.0f * m_boxHalfExtZ);
            break;
            
        case ShapeType::Capsule:
            // Volume = π * r² * h + (4/3) * π * r³ (cylinder + hemispheres)
            // Height is total height, cylinder height = h - 2r
            {
                float cylinderHeight = m_capsuleHeight - 2.0f * m_capsuleRadius;
                if (cylinderHeight < 0.0f) cylinderHeight = 0.0f;
                float cylinderVolume = PI * m_capsuleRadius * m_capsuleRadius * cylinderHeight;
                float hemisphereVolume = (4.0f / 3.0f) * PI * m_capsuleRadius * m_capsuleRadius * m_capsuleRadius;
                volume = cylinderVolume + hemisphereVolume;
            }
            break;
            
        case ShapeType::Cylinder:
            // Volume = π * r² * h
            volume = PI * m_capsuleRadius * m_capsuleRadius * m_capsuleHeight;
            break;
            
        case ShapeType::ConvexHull:
        case ShapeType::TriangleMesh:
        case ShapeType::Heightfield:
            // Complex shapes require user-specified mass
            volume = 1.0f;
            break;
    }
    
    // Mass = Volume * Density
    float mass = volume * m_material.density;
    
    // Clamp to valid range
    if (mass < MIN_MASS) mass = MIN_MASS;
    if (mass > MAX_MASS) mass = MAX_MASS;
    
    return mass;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
