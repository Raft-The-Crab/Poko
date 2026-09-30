/**
 * @file pair_algorithms.cpp
 * @brief Pair collision algorithms implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "../../../include/core/components/physics/narrowphase/pair_algorithms/pair_algorithms.h"
#include <algorithm>
#include <variant>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

CollisionResult collideSphereSphere(
    const Sphere& sphereA,
    const Transform& transformA,
    const Sphere& sphereB,
    const Transform& transformB
) {
    CollisionResult result;
    
    Vector3 posA = transformA.position;
    Vector3 posB = transformB.position;
    
    Vector3 diff = posB - posA;
    float distance = diff.length();
    float radiusSum = sphereA.radius + sphereB.radius;
    
    if (distance < radiusSum) {
        result.isColliding = true;
        result.penetration = radiusSum - distance;
        
        if (distance > 0.0001f) {
            result.normal = diff.normalized();
        } else {
            result.normal = Vector3(0.0f, 1.0f, 0.0f);
        }
        
        ContactPoint contact;
        contact.position = posA + result.normal * (sphereA.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        result.contacts.push_back(contact);
    }
    
    return result;
}

CollisionResult collideSphereBox(
    const Sphere& sphere,
    const Transform& sphereTransform,
    const Box& box,
    const Transform& boxTransform
) {
    CollisionResult result;
    
    // Transform sphere position to box local space
    Vector3 spherePos = sphereTransform.position;
    Vector3 boxPos = boxTransform.position;
    Quaternion boxRot = boxTransform.rotation;
    
    Vector3 localSpherePos = boxRot.inverse().rotateVector(spherePos - boxPos);
    
    // Find closest point on box to sphere center
    Vector3 closest = localSpherePos;
    closest.x = std::max(-box.halfExtents.x, std::min(box.halfExtents.x, closest.x));
    closest.y = std::max(-box.halfExtents.y, std::min(box.halfExtents.y, closest.y));
    closest.z = std::max(-box.halfExtents.z, std::min(box.halfExtents.z, closest.z));
    
    Vector3 diff = localSpherePos - closest;
    float distance = diff.length();
    
    if (distance < sphere.radius) {
        result.isColliding = true;
        result.penetration = sphere.radius - distance;
        
        if (distance > 0.0001f) {
            Vector3 localNormal = diff.normalized();
            result.normal = boxRot.rotateVector(localNormal);
        } else {
            result.normal = Vector3(0.0f, 1.0f, 0.0f);
        }
        
        ContactPoint contact;
        contact.position = spherePos - result.normal * (sphere.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        result.contacts.push_back(contact);
    }
    
    return result;
}

CollisionResult collideBoxBox(
    const Box& boxA,
    const Transform& transformA,
    const Box& boxB,
    const Transform& transformB
) {
    CollisionResult result;
    
    // Simplified AABB collision for now
    // Full implementation would use SAT with face and edge axes
    
    Transform worldToA = transformA.inverse();
    Transform bInA = worldToA.compose(transformB);
    
    Vector3 centerBInA = bInA.position;
    Vector3 halfExtentsA = boxA.halfExtents;
    Vector3 halfExtentsB = boxB.halfExtents;
    
    Vector3 diff = centerBInA - Vector3::zero();
    Vector3 absDiff = diff.abs();
    
    float overlapX = halfExtentsA.x + halfExtentsB.x - absDiff.x;
    float overlapY = halfExtentsA.y + halfExtentsB.y - absDiff.y;
    float overlapZ = halfExtentsA.z + halfExtentsB.z - absDiff.z;
    
    if (overlapX > 0 && overlapY > 0 && overlapZ > 0) {
        result.isColliding = true;
        
        // Find minimum overlap axis
        if (overlapX < overlapY && overlapX < overlapZ) {
            result.penetration = overlapX;
            result.normal = diff.x > 0 ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(-1.0f, 0.0f, 0.0f);
        } else if (overlapY < overlapZ) {
            result.penetration = overlapY;
            result.normal = diff.y > 0 ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, -1.0f, 0.0f);
        } else {
            result.penetration = overlapZ;
            result.normal = diff.z > 0 ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 0.0f, -1.0f);
        }
        
        result.normal = transformA.rotation.rotateVector(result.normal);
        
        ContactPoint contact;
        contact.position = transformA.position + result.normal * (result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        result.contacts.push_back(contact);
    }
    
    return result;
}

CollisionResult collideSpherePlane(
    const Sphere& sphere,
    const Transform& sphereTransform,
    const Plane& plane,
    const Transform& planeTransform
) {
    CollisionResult result;
    
    Vector3 spherePos = sphereTransform.position;
    Vector3 planeNormal = planeTransform.rotation.rotateVector(plane.normal);
    
    float distance = planeNormal.dot(spherePos) + plane.constant;
    
    if (distance < sphere.radius) {
        result.isColliding = true;
        result.penetration = sphere.radius - distance;
        result.normal = planeNormal;
        
        ContactPoint contact;
        contact.position = spherePos - planeNormal * (sphere.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        result.contacts.push_back(contact);
    }
    
    return result;
}

CollisionResult collideCapsulePlane(
    const Capsule& capsule,
    const Transform& capsuleTransform,
    const Plane& plane,
    const Transform& planeTransform
) {
    CollisionResult result;
    
    // Simplified: treat as sphere at center
    // Full implementation would check both sphere ends
    
    Vector3 capsulePos = capsuleTransform.position;
    Vector3 planeNormal = planeTransform.rotation.rotateVector(plane.normal);
    
    float distance = planeNormal.dot(capsulePos) + plane.constant;
    
    if (distance < capsule.radius) {
        result.isColliding = true;
        result.penetration = capsule.radius - distance;
        result.normal = planeNormal;
        
        ContactPoint contact;
        contact.position = capsulePos - planeNormal * (capsule.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        result.contacts.push_back(contact);
    }
    
    return result;
}

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
