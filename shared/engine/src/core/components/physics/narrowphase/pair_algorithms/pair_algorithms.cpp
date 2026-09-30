/**
 * @file pair_algorithms.cpp
 * @brief Pair collision algorithms implementation
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/narrowphase/pair_algorithms/pair_algorithms.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"
#include "core/components/physics/matrices/matrix3x3.h"
#include <algorithm>
#include <cmath>
#include <variant>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

using math::Vector3;
using math::Quaternion;
using matrices::Matrix3x3;

/**
 * @brief Generate feature ID for contact persistence
 */
constexpr uint32_t generateFeatureId(uint32_t shapeType, uint32_t index) {
    return (shapeType << 24) | index;
}

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
    float distanceSquared = diff.lengthSquared();
    float radiusSum = sphereA.radius + sphereB.radius;
    float radiusSumSquared = radiusSum * radiusSum;

    if (distanceSquared < radiusSumSquared) {
        result.isColliding = true;

        float distance = std::sqrt(distanceSquared);
        result.penetration = radiusSum - distance;

        if (distance > 0.0001f) {
            result.normal = diff * (1.0f / distance);
        } else {
            result.normal = Vector3(0.0f, 1.0f, 0.0f);
        }

        ContactPoint contact;
        contact.position = posA + result.normal * (sphereA.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        contact.featureIdA = generateFeatureId(1, 0); // Sphere type 1
        contact.featureIdB = generateFeatureId(1, 0);
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

    Vector3 relPos = spherePos - boxPos;
    Vector3 localSpherePos = boxRot.inverse().rotateVector(relPos);

    // Find closest point on box to sphere center
    Vector3 closest = localSpherePos;
    closest.x = std::max(-box.halfExtents.x, std::min(box.halfExtents.x, closest.x));
    closest.y = std::max(-box.halfExtents.y, std::min(box.halfExtents.y, closest.y));
    closest.z = std::max(-box.halfExtents.z, std::min(box.halfExtents.z, closest.z));

    Vector3 diff = localSpherePos - closest;
    float distanceSquared = diff.lengthSquared();

    if (distanceSquared < sphere.radius * sphere.radius) {
        result.isColliding = true;

        float distance = std::sqrt(distanceSquared);
        result.penetration = sphere.radius - distance;

        Vector3 localNormal;
        if (distance > 0.0001f) {
            localNormal = diff * (1.0f / distance);
        } else {
            // Sphere center is inside box, use closest face normal
            Vector3 absLocal = localSpherePos.abs();
            if (absLocal.x > absLocal.y && absLocal.x > absLocal.z) {
                localNormal = localSpherePos.x > 0 ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(-1.0f, 0.0f, 0.0f);
            } else if (absLocal.y > absLocal.z) {
                localNormal = localSpherePos.y > 0 ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, -1.0f, 0.0f);
            } else {
                localNormal = localSpherePos.z > 0 ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 0.0f, -1.0f);
            }
        }

        result.normal = boxRot.rotateVector(localNormal);

        ContactPoint contact;
        contact.position = spherePos - result.normal * (sphere.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        contact.featureIdA = generateFeatureId(1, 0); // Sphere
        contact.featureIdB = generateFeatureId(2, 0); // Box face
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

    // Production-grade OBB collision using SAT (Separating Axis Theorem)
    // Test 15 axes: 3 face normals from A, 3 from B, 9 edge cross products

    Matrix3x3 rotA = Matrix3x3::fromQuaternion(transformA.rotation);
    Matrix3x3 rotB = Matrix3x3::fromQuaternion(transformB.rotation);

    // Rotation matrix from B to A
    Matrix3x3 rotBToA = rotA.transpose() * rotB;

    // Translation from B to A in A's frame
    Vector3 posA = transformA.position;
    Vector3 posB = transformB.position;
    Vector3 translation = posB - posA;
    Vector3 translationA = rotA.transpose() * translation;

    // Face normals from A
    Vector3 axesA[3] = {
        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f),
        Vector3(0.0f, 0.0f, 1.0f)
    };

    // Face normals from B in A's frame
    Vector3 axesB[3] = {
        rotBToA.getColumn(0),
        rotBToA.getColumn(1),
        rotBToA.getColumn(2)
    };

    // Test A's face normals
    float minOverlap = 1e30f;
    Vector3 minAxis = Vector3::zero();

    for (int i = 0; i < 3; ++i) {
        float projection = std::abs(translationA.dot(axesA[i]));
        float halfExtentA = boxA.halfExtents.getComponent(i);
        float halfExtentB = boxB.halfExtents.x * std::abs(axesB[0].dot(axesA[i])) +
                           boxB.halfExtents.y * std::abs(axesB[1].dot(axesA[i])) +
                           boxB.halfExtents.z * std::abs(axesB[2].dot(axesA[i]));

        float overlap = halfExtentA + halfExtentB - projection;
        if (overlap < 0) {
            return result; // Separating axis found
        }

        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axesA[i];
        }
    }

    // Test B's face normals
    for (int i = 0; i < 3; ++i) {
        float projection = std::abs(translationA.dot(axesB[i]));
        float halfExtentA = boxA.halfExtents.x * std::abs(axesA[0].dot(axesB[i])) +
                           boxA.halfExtents.y * std::abs(axesA[1].dot(axesB[i])) +
                           boxA.halfExtents.z * std::abs(axesA[2].dot(axesB[i]));
        float halfExtentB = boxB.halfExtents.getComponent(i);

        float overlap = halfExtentA + halfExtentB - projection;
        if (overlap < 0) {
            return result; // Separating axis found
        }

        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axesB[i];
        }
    }

    // Test edge cross products (9 axes)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Vector3 axis = axesA[i].cross(axesB[j]);
            float axisLengthSquared = axis.lengthSquared();

            if (axisLengthSquared < 0.0001f) {
                continue; // Parallel edges
            }

            axis = axis * (1.0f / std::sqrt(axisLengthSquared));

            float projection = std::abs(translationA.dot(axis));
            float halfExtentA = boxA.halfExtents.x * std::abs(axesA[0].dot(axis)) +
                               boxA.halfExtents.y * std::abs(axesA[1].dot(axis)) +
                               boxA.halfExtents.z * std::abs(axesA[2].dot(axis));
            float halfExtentB = boxB.halfExtents.x * std::abs(axesB[0].dot(axis)) +
                               boxB.halfExtents.y * std::abs(axesB[1].dot(axis)) +
                               boxB.halfExtents.z * std::abs(axesB[2].dot(axis));

            float overlap = halfExtentA + halfExtentB - projection;
            if (overlap < 0) {
                return result; // Separating axis found
            }

            if (overlap < minOverlap) {
                minOverlap = overlap;
                minAxis = axis;
            }
        }
    }

    // No separating axis found - collision detected
    result.isColliding = true;
    result.penetration = minOverlap;

    // Ensure normal points from A to B
    if (translation.dot(minAxis) < 0) {
        minAxis = -minAxis;
    }

    result.normal = transformA.rotation.rotateVector(minAxis);

    ContactPoint contact;
    contact.position = posA + result.normal * (minOverlap * 0.5f);
    contact.normal = result.normal;
    contact.penetration = result.penetration;
    contact.featureIdA = generateFeatureId(2, 0); // Box face
    contact.featureIdB = generateFeatureId(2, 0);
    result.contacts.push_back(contact);

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

    // Distance from sphere center to plane
    float distance = planeNormal.dot(spherePos) + plane.constant;

    if (distance < sphere.radius) {
        result.isColliding = true;
        result.penetration = sphere.radius - distance;
        result.normal = planeNormal;

        ContactPoint contact;
        contact.position = spherePos - planeNormal * (sphere.radius - result.penetration * 0.5f);
        contact.normal = result.normal;
        contact.penetration = result.penetration;
        contact.featureIdA = generateFeatureId(1, 0); // Sphere
        contact.featureIdB = generateFeatureId(4, 0); // Plane
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

    Vector3 capsulePos = capsuleTransform.position;
    Vector3 planeNormal = planeTransform.rotation.rotateVector(plane.normal);

    // Calculate capsule segment endpoints
    Vector3 capsuleUp = capsuleTransform.rotation.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
    Vector3 halfHeight = capsuleUp * (capsule.height * 0.5f);
    Vector3 endpointA = capsulePos - halfHeight;
    Vector3 endpointB = capsulePos + halfHeight;

    // Check both endpoints
    float distanceA = planeNormal.dot(endpointA) + plane.constant;
    float distanceB = planeNormal.dot(endpointB) + plane.constant;

    bool collidingA = distanceA < capsule.radius;
    bool collidingB = distanceB < capsule.radius;

    if (collidingA || collidingB) {
        result.isColliding = true;

        // Use the deepest penetration
        float penetrationA = capsule.radius - distanceA;
        float penetrationB = capsule.radius - distanceB;

        if (penetrationA > penetrationB) {
            result.penetration = penetrationA;
            ContactPoint contact;
            contact.position = endpointA - planeNormal * (capsule.radius - result.penetration * 0.5f);
            contact.normal = planeNormal;
            contact.penetration = result.penetration;
            contact.featureIdA = generateFeatureId(3, 0); // Capsule endpoint A
            contact.featureIdB = generateFeatureId(4, 0); // Plane
            result.contacts.push_back(contact);
        } else {
            result.penetration = penetrationB;
            ContactPoint contact;
            contact.position = endpointB - planeNormal * (capsule.radius - result.penetration * 0.5f);
            contact.normal = planeNormal;
            contact.penetration = result.penetration;
            contact.featureIdA = generateFeatureId(3, 1); // Capsule endpoint B
            contact.featureIdB = generateFeatureId(4, 0); // Plane
            result.contacts.push_back(contact);
        }
    }

    return result;
}

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
