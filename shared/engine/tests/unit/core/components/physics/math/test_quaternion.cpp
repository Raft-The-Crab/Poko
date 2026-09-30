/**
 * @file test_quaternion.cpp
 * @brief Quaternion unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/math/quaternion.h"
#include "core/components/physics/math/vector3.h"
#include <cassert>
#include <cmath>

using namespace poko::core::components::physics::math;

void test_quaternion_constants() {
    assert(Quaternion::IDENTITY.isIdentity());
    assert(Quaternion::ZERO.w == 0.0f);
}

void test_quaternion_from_axis_angle() {
    Quaternion q = Quaternion::fromAxisAngle(Vector3::UP, 3.14159f); // 180 degrees
    assert(std::abs(q.w) < 0.01f); // cos(90°) ≈ 0
    assert(std::abs(q.length() - 1.0f) < 0.0001f);
}

void test_quaternion_from_euler() {
    Quaternion q = Quaternion::fromEuler(0.0f, 0.0f, 0.0f);
    assert(q.isIdentity());
}

void test_quaternion_multiplication() {
    Quaternion a = Quaternion::fromAxisAngle(Vector3::UP, 0.5f);
    Quaternion b = Quaternion::fromAxisAngle(Vector3::UP, 0.5f);
    Quaternion c = a * b;
    assert(std::abs(c.length() - 1.0f) < 0.0001f);
}

void test_quaternion_scalar_multiplication() {
    Quaternion q = Quaternion::IDENTITY;
    Quaternion result = q * 2.0f;
    assert(result.w == 2.0f);
}

void test_quaternion_addition() {
    Quaternion a = Quaternion::IDENTITY;
    Quaternion b = Quaternion::IDENTITY;
    Quaternion result = a + b;
    assert(result.w == 2.0f);
}

void test_quaternion_dot_product() {
    Quaternion a = Quaternion::IDENTITY;
    Quaternion b = Quaternion::IDENTITY;
    float dot = a.dot(b);
    assert(std::abs(dot - 1.0f) < 0.0001f);
}

void test_quaternion_length() {
    Quaternion q = Quaternion::IDENTITY;
    assert(std::abs(q.length() - 1.0f) < 0.0001f);
    assert(std::abs(q.lengthSquared() - 1.0f) < 0.0001f);
}

void test_quaternion_normalize() {
    Quaternion q(2.0f, 0.0f, 0.0f, 0.0f);
    Quaternion normalized = q.normalized();
    assert(std::abs(normalized.length() - 1.0f) < 0.0001f);
}

void test_quaternion_conjugate() {
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion conj = q.conjugate();
    assert(conj.w == 1.0f);
    assert(conj.x == -2.0f);
    assert(conj.y == -3.0f);
    assert(conj.z == -4.0f);
}

void test_quaternion_inverse() {
    Quaternion q = Quaternion::IDENTITY;
    Quaternion inv = q.inverse();
    assert(inv.isIdentity());
}

void test_quaternion_rotate_vector() {
    Quaternion q = Quaternion::fromAxisAngle(Vector3::UP, 1.5708f); // 90 degrees
    Vector3 v(1.0f, 0.0f, 0.0f);
    Vector3 rotated = q.rotateVector(v);
    // X rotated 90° around Y should become -Z
    assert(std::abs(rotated.x) < 0.01f);
    assert(std::abs(rotated.y) < 0.01f);
    assert(std::abs(rotated.z + 1.0f) < 0.01f);
}

void test_quaternion_slerp() {
    Quaternion a = Quaternion::IDENTITY;
    Quaternion b = Quaternion::fromAxisAngle(Vector3::UP, 1.5708f);
    Quaternion mid = Quaternion::slerp(a, b, 0.5f);
    assert(mid.isNormalized());
}

void test_quaternion_lerp() {
    Quaternion a = Quaternion::IDENTITY;
    Quaternion b = Quaternion::fromAxisAngle(Vector3::UP, 1.5708f);
    Quaternion mid = Quaternion::lerp(a, b, 0.5f);
    assert(mid.isNormalized());
}

void test_quaternion_identity_check() {
    assert(Quaternion::IDENTITY.isIdentity());
    Quaternion q(1.0f, 0.01f, 0.0f, 0.0f);
    assert(!q.isIdentity(0.001f));
}

void test_quaternion_normalized_check() {
    assert(Quaternion::IDENTITY.isNormalized());
    Quaternion q(2.0f, 0.0f, 0.0f, 0.0f);
    assert(!q.isNormalized());
}

void test_quaternion_get_axis_angle() {
    Quaternion q = Quaternion::fromAxisAngle(Vector3::UP, 1.5708f);
    Vector3 axis = q.getAxis();
    float angle = q.getAngle();
    assert(std::abs(axis.y - 1.0f) < 0.01f);
    assert(std::abs(angle - 1.5708f) < 0.01f);
}

int main() {
    test_quaternion_constants();
    test_quaternion_from_axis_angle();
    test_quaternion_from_euler();
    test_quaternion_multiplication();
    test_quaternion_scalar_multiplication();
    test_quaternion_addition();
    test_quaternion_dot_product();
    test_quaternion_length();
    test_quaternion_normalize();
    test_quaternion_conjugate();
    test_quaternion_inverse();
    test_quaternion_rotate_vector();
    test_quaternion_slerp();
    test_quaternion_lerp();
    test_quaternion_identity_check();
    test_quaternion_normalized_check();
    test_quaternion_get_axis_angle();
    
    return 0;
}
