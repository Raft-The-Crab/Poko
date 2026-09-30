/**
 * @file narrowphase.cpp
 * @brief Narrowphase collision detection implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/narrowphase/narrowphase.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

using math::Vector3;
using shapes::Sphere;
using shapes::Box;
using shapes::Capsule;

bool Narrowphase::collideSphereSphere(
    const shapes::Sphere& a,
    const shapes::Sphere& b,
    ContactManifold& manifold
) {
    manifold.clear();
    
    math::Vector3 diff = b.center - a.center;
    float distSq = diff.lengthSquared();
    float radiusSum = a.radius + b.radius;
    
    if (distSq > radiusSum * radiusSum) {
        return false;
    }
    
    float dist = std::sqrt(distSq);
    if (dist < 0.0001f) {
        // Spheres are at the same position
        manifold.bodyA = 0;
        manifold.bodyB = 0;
        
        ContactPoint contact;
        contact.position = a.center;
        contact.normal = math::Vector3::up();
        contact.penetration = radiusSum;
        manifold.addContact(contact);
        return true;
    }
    
    manifold.bodyA = 0;
    manifold.bodyB = 0;
    
    ContactPoint contact;
    contact.normal = diff / dist;
    contact.position = a.center + contact.normal * a.radius;
    contact.penetration = radiusSum - dist;
    manifold.addContact(contact);
    
    return true;
}

bool Narrowphase::collideSphereBox(
    const shapes::Sphere& sphere,
    const shapes::Box& box,
    ContactManifold& manifold
) {
    manifold.clear();
    
    // Transform sphere center to box local space
    math::Vector3 localCenter = box.orientation.transpose() * (sphere.center - box.center);
    
    // Find closest point on box to sphere center
    math::Vector3 closestPoint(
        localCenter.x > box.halfExtents.x ? box.halfExtents.x : (localCenter.x < -box.halfExtents.x ? -box.halfExtents.x : localCenter.x),
        localCenter.y > box.halfExtents.y ? box.halfExtents.y : (localCenter.y < -box.halfExtents.y ? -box.halfExtents.y : localCenter.y),
        localCenter.z > box.halfExtents.z ? box.halfExtents.z : (localCenter.z < -box.halfExtents.z ? -box.halfExtents.z : localCenter.z)
    );
    
    math::Vector3 diff = localCenter - closestPoint;
    float distSq = diff.lengthSquared();
    
    if (distSq > sphere.radius * sphere.radius) {
        return false;
    }
    
    manifold.bodyA = 0;
    manifold.bodyB = 0;
    
    ContactPoint contact;
    
    if (distSq < 0.0001f) {
        // Sphere center is inside box
        contact.normal = (box.orientation * (localCenter - closestPoint)).normalized();
        contact.position = sphere.center;
        contact.penetration = sphere.radius;
    } else {
        float dist = std::sqrt(distSq);
        math::Vector3 localNormal = diff / dist;
        contact.normal = box.orientation * localNormal;
        contact.position = box.center + box.orientation * closestPoint;
        contact.penetration = sphere.radius - dist;
    }
    
    manifold.addContact(contact);
    return true;
}

bool Narrowphase::collideBoxBox(
    const shapes::Box& a,
    const shapes::Box& b,
    ContactManifold& manifold
) {
    manifold.clear();
    
    // SAT (Separating Axis Theorem) implementation
    // Test face normals of both boxes
    math::Vector3 normalsA[6];
    a.getNormals(normalsA);
    
    math::Vector3 normalsB[6];
    b.getNormals(normalsB);
    
    // Test all 15 axes (6 + 6 + 3 cross products)
    // For now, just test face normals (simplified)
    float minOverlap = 1e30f;
    math::Vector3 bestNormal;
    
    for (int i = 0; i < 6; ++i) {
        math::Vector3 axis = normalsA[i];
        
        // Project both boxes onto axis
        float minA = 1e30f, maxA = -1e30f;
        float minB = 1e30f, maxB = -1e30f;
        
        math::Vector3 cornersA[8];
        a.getCorners(cornersA);
        
        for (int j = 0; j < 8; ++j) {
            float proj = cornersA[j].dot(axis);
            minA = std::min(minA, proj);
            maxA = std::max(maxA, proj);
        }
        
        math::Vector3 cornersB[8];
        b.getCorners(cornersB);
        
        for (int j = 0; j < 8; ++j) {
            float proj = cornersB[j].dot(axis);
            minB = std::min(minB, proj);
            maxB = std::max(maxB, proj);
        }
        
        // Check for separation
        if (maxA < minB || maxB < minA) {
            return false;
        }
        
        // Calculate overlap
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestNormal = axis;
        }
    }
    
    manifold.bodyA = 0;
    manifold.bodyB = 0;
    
    ContactPoint contact;
    contact.normal = bestNormal;
    contact.penetration = minOverlap;
    contact.position = (a.center + b.center) * 0.5f;
    manifold.addContact(contact);
    
    return true;
}

bool Narrowphase::collideCapsuleSphere(
    const shapes::Capsule& capsule,
    const shapes::Sphere& sphere,
    ContactManifold& manifold
) {
    manifold.clear();
    
    // Get capsule endpoints
    math::Vector3 p1, p2;
    capsule.getEndpoints(p1, p2);
    
    // Find closest point on capsule line segment to sphere center
    math::Vector3 segment = p2 - p1;
    float segmentLenSq = segment.lengthSquared();
    
    math::Vector3 diff = sphere.center - p1;
    float t = diff.dot(segment) / segmentLenSq;
    t = std::clamp(t, 0.0f, 1.0f);
    
    math::Vector3 closestPoint = p1 + segment * t;
    
    float distSq = sphere.center.distanceSquaredTo(closestPoint);
    float radiusSum = capsule.radius + sphere.radius;
    
    if (distSq > radiusSum * radiusSum) {
        return false;
    }
    
    manifold.bodyA = 0;
    manifold.bodyB = 0;
    
    ContactPoint contact;
    float dist = std::sqrt(distSq);
    
    if (dist < 0.0001f) {
        contact.normal = math::Vector3::up();
        contact.position = sphere.center;
        contact.penetration = radiusSum;
    } else {
        contact.normal = (sphere.center - closestPoint) / dist;
        contact.position = closestPoint + contact.normal * capsule.radius;
        contact.penetration = radiusSum - dist;
    }
    
    manifold.addContact(contact);
    return true;
}

bool Narrowphase::collideCapsuleBox(
    const shapes::Capsule& capsule,
    const shapes::Box& box,
    ContactManifold& manifold
) {
    manifold.clear();
    
    // Simplified: treat capsule as line of spheres
    // For production, implement GJK or capsule-specific algorithm
    
    math::Vector3 p1, p2;
    capsule.getEndpoints(p1, p2);
    
    // Sample points along capsule and test against box
    int samples = 10;
    float step = 1.0f / (samples - 1);
    
    bool hasContact = false;
    
    for (int i = 0; i < samples; ++i) {
        float t = i * step;
        math::Vector3 point = p1 + (p2 - p1) * t;
        
        shapes::Sphere tempSphere(point, capsule.radius);
        ContactManifold tempManifold;
        
        if (collideSphereBox(tempSphere, box, tempManifold)) {
            if (!hasContact) {
                manifold = tempManifold;
                hasContact = true;
            } else {
                // Merge contacts
                for (uint32_t j = 0; j < tempManifold.contactCount; ++j) {
                    manifold.addContact(tempManifold.contacts[j]);
                }
            }
        }
    }
    
    return hasContact;
}

bool Narrowphase::collideCapsuleCapsule(
    const shapes::Capsule& a,
    const shapes::Capsule& b,
    ContactManifold& manifold
) {
    manifold.clear();
    
    // Get capsule endpoints
    math::Vector3 a1, a2;
    a.getEndpoints(a1, a2);
    
    math::Vector3 b1, b2;
    b.getEndpoints(b1, b2);
    
    // Find closest points between two line segments
    math::Vector3 segA = a2 - a1;
    math::Vector3 segB = b2 - b1;
    
    float segALenSq = segA.lengthSquared();
    float segBLenSq = segB.lengthSquared();
    
    math::Vector3 diff = a1 - b1;
    
    float dA = diff.dot(segA);
    float dB = diff.dot(segB);
    
    float denom = segALenSq * segBLenSq - dA * dA;
    
    float tA, tB;
    
    if (std::abs(denom) < 0.0001f) {
        // Parallel segments
        tA = 0.0f;
        tB = dB / segBLenSq;
    } else {
        tA = (dB * dA - dA * segBLenSq) / denom;
        tB = (tA * dA + dB) / segBLenSq;
    }
    
    tA = std::clamp(tA, 0.0f, 1.0f);
    tB = std::clamp(tB, 0.0f, 1.0f);
    
    math::Vector3 closestA = a1 + segA * tA;
    math::Vector3 closestB = b1 + segB * tB;
    
    float distSq = closestA.distanceSquaredTo(closestB);
    float radiusSum = a.radius + b.radius;
    
    if (distSq > radiusSum * radiusSum) {
        return false;
    }
    
    manifold.bodyA = 0;
    manifold.bodyB = 0;
    
    ContactPoint contact;
    float dist = std::sqrt(distSq);
    
    if (dist < 0.0001f) {
        contact.normal = math::Vector3::up();
        contact.position = closestA;
        contact.penetration = radiusSum;
    } else {
        contact.normal = (closestA - closestB) / dist;
        contact.position = closestB + contact.normal * b.radius;
        contact.penetration = radiusSum - dist;
    }
    
    manifold.addContact(contact);
    return true;
}

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
