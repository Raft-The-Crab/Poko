/**
 * @file test_vector3.cpp
 * @brief Vector3 unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/math/vector3.h"
#include <cassert>
#include <cmath>

using namespace poko::core::components::physics::math;

void test_vector3_constants() {
    assert(Vector3::ZERO.x == 0.0f);
    assert(Vector3::ZERO.y == 0.0f);
    assert(Vector3::ZERO.z == 0.0f);
    
    assert(Vector3::ONE.x == 1.0f);
    assert(Vector3::ONE.y == 1.0f);
    assert(Vector3::ONE.z == 1.0f);
    
    assert(Vector3::UP.x == 0.0f);
    assert(Vector3::UP.y == 1.0f);
    assert(Vector3::UP.z == 0.0f);
    
    assert(Vector3::FORWARD.x == 0.0f);
    assert(Vector3::FORWARD.y == 0.0f);
    assert(Vector3::FORWARD.z == 1.0f);
    
    assert(Vector3::RIGHT.x == 1.0f);
    assert(Vector3::RIGHT.y == 0.0f);
    assert(Vector3::RIGHT.z == 0.0f);
}

void test_vector3_arithmetic() {
    Vector3 a(1.0f, 2.0f, 3.0f);
    Vector3 b(4.0f, 5.0f, 6.0f);
    
    // Addition
    Vector3 c = a + b;
    assert(c.x == 5.0f);
    assert(c.y == 7.0f);
    assert(c.z == 9.0f);
    
    // Subtraction
    Vector3 d = b - a;
    assert(d.x == 3.0f);
    assert(d.y == 3.0f);
    assert(d.z == 3.0f);
    
    // Scalar multiplication
    Vector3 e = a * 2.0f;
    assert(e.x == 2.0f);
    assert(e.y == 4.0f);
    assert(e.z == 6.0f);
    
    // Scalar division
    Vector3 f = a / 2.0f;
    assert(f.x == 0.5f);
    assert(f.y == 1.0f);
    assert(f.z == 1.5f);
    
    // Negation
    Vector3 g = -a;
    assert(g.x == -1.0f);
    assert(g.y == -2.0f);
    assert(g.z == -3.0f);
}

void test_vector3_dot_product() {
    Vector3 a(1.0f, 0.0f, 0.0f);
    Vector3 b(0.0f, 1.0f, 0.0f);
    
    // Perpendicular vectors
    float dot = a.dot(b);
    assert(std::abs(dot) < 0.0001f);
    
    // Parallel vectors
    Vector3 c(1.0f, 0.0f, 0.0f);
    Vector3 d(2.0f, 0.0f, 0.0f);
    dot = c.dot(d);
    assert(std::abs(dot - 2.0f) < 0.0001f);
}

void test_vector3_cross_product() {
    Vector3 a(1.0f, 0.0f, 0.0f);
    Vector3 b(0.0f, 1.0f, 0.0f);
    
    // Cross product of X and Y should be Z
    Vector3 c = a.cross(b);
    assert(std::abs(c.x) < 0.0001f);
    assert(std::abs(c.y) < 0.0001f);
    assert(std::abs(c.z - 1.0f) < 0.0001f);
    
    // Cross product of Y and X should be -Z
    Vector3 d = b.cross(a);
    assert(std::abs(d.x) < 0.0001f);
    assert(std::abs(d.y) < 0.0001f);
    assert(std::abs(d.z + 1.0f) < 0.0001f);
}

void test_vector3_length() {
    Vector3 a(3.0f, 4.0f, 0.0f);
    
    // Length squared
    float lenSq = a.lengthSquared();
    assert(std::abs(lenSq - 25.0f) < 0.0001f);
    
    // Length
    float len = a.length();
    assert(std::abs(len - 5.0f) < 0.0001f);
}

void test_vector3_normalize() {
    Vector3 a(3.0f, 4.0f, 0.0f);
    
    // Normalize
    Vector3 normalized = a.normalized();
    assert(std::abs(normalized.length() - 1.0f) < 0.0001f);
    
    // Normalize in place
    Vector3 b(0.0f, 5.0f, 0.0f);
    b.normalize();
    assert(std::abs(b.length() - 1.0f) < 0.0001f);
}

void test_vector3_distance() {
    Vector3 a(0.0f, 0.0f, 0.0f);
    Vector3 b(3.0f, 4.0f, 0.0f);
    
    // Distance
    float dist = a.distanceTo(b);
    assert(std::abs(dist - 5.0f) < 0.0001f);
    
    // Distance squared
    float distSq = a.distanceSquaredTo(b);
    assert(std::abs(distSq - 25.0f) < 0.0001f);
}

void test_vector3_zero_check() {
    Vector3 a(0.0f, 0.0f, 0.0f);
    assert(a.isZero());
    
    Vector3 b(0.00001f, 0.0f, 0.0f);
    assert(b.isZero(0.0001f));
    
    Vector3 c(1.0f, 0.0f, 0.0f);
    assert(!c.isZero());
}

void test_vector3_normalized_check() {
    Vector3 a(1.0f, 0.0f, 0.0f);
    assert(a.isNormalized());
    
    Vector3 b(0.0f, 1.0f, 0.0f);
    assert(b.isNormalized());
    
    Vector3 c(1.0f, 1.0f, 0.0f);
    assert(!c.isNormalized());
}

int main() {
    test_vector3_constants();
    test_vector3_arithmetic();
    test_vector3_dot_product();
    test_vector3_cross_product();
    test_vector3_length();
    test_vector3_normalize();
    test_vector3_distance();
    test_vector3_zero_check();
    test_vector3_normalized_check();
    
    return 0;
}
