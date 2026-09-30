/**
 * @file test_matrix3x3.cpp
 * @brief Matrix3x3 unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/math/matrix3x3.h"
#include "core/components/physics/math/vector3.h"
#include <cassert>
#include <cmath>

using namespace poko::core::components::physics::math;

void test_matrix3x3_constants() {
    assert(Matrix3x3::ZERO.isZero());
    assert(Matrix3x3::IDENTITY.isIdentity());
}

void test_matrix3x3_diagonal() {
    Matrix3x3 m = Matrix3x3::diagonal(1.0f, 2.0f, 3.0f);
    assert(m.m00 == 1.0f);
    assert(m.m04 == 2.0f);
    assert(m.m08 == 3.0f);
    assert(m.m01 == 0.0f);
    assert(m.m02 == 0.0f);
    assert(m.m03 == 0.0f);
    assert(m.m05 == 0.0f);
    assert(m.m06 == 0.0f);
    assert(m.m07 == 0.0f);
}

void test_matrix3x3_inertia_tensor() {
    Matrix3x3 m = Matrix3x3::inertiaTensor(1.0f, 2.0f, 3.0f);
    assert(m.m00 == 1.0f);
    assert(m.m04 == 2.0f);
    assert(m.m08 == 3.0f);
}

void test_matrix3x3_scalar_multiplication() {
    Matrix3x3 m = Matrix3x3::IDENTITY;
    Matrix3x3 result = m * 2.0f;
    assert(result.m00 == 2.0f);
    assert(result.m04 == 2.0f);
    assert(result.m08 == 2.0f);
}

void test_matrix3x3_addition() {
    Matrix3x3 a = Matrix3x3::IDENTITY;
    Matrix3x3 b = Matrix3x3::IDENTITY;
    Matrix3x3 result = a + b;
    assert(result.m00 == 2.0f);
    assert(result.m04 == 2.0f);
    assert(result.m08 == 2.0f);
}

void test_matrix3x3_transpose() {
    Matrix3x3 m(
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    );
    Matrix3x3 t = m.transpose();
    assert(t.m00 == 1.0f);
    assert(t.m01 == 4.0f);
    assert(t.m02 == 7.0f);
    assert(t.m03 == 2.0f);
    assert(t.m04 == 5.0f);
    assert(t.m05 == 8.0f);
    assert(t.m06 == 3.0f);
    assert(t.m07 == 6.0f);
    assert(t.m08 == 9.0f);
}

void test_matrix3x3_determinant() {
    Matrix3x3 m = Matrix3x3::IDENTITY;
    float det = m.determinant();
    assert(std::abs(det - 1.0f) < 0.0001f);
}

void test_matrix3x3_inverse() {
    Matrix3x3 m = Matrix3x3::IDENTITY;
    Matrix3x3 inv = m.inverse();
    assert(inv.isIdentity());
}

void test_matrix3x3_cross_product_matrix() {
    Vector3 v(1.0f, 0.0f, 0.0f);
    Matrix3x3 m = Matrix3x3::crossProductMatrix(v);
    assert(m.m00 == 0.0f);
    assert(m.m01 == 0.0f);
    assert(m.m02 == 0.0f);
    assert(m.m03 == 0.0f);
    assert(m.m04 == 0.0f);
    assert(m.m05 == -1.0f);
    assert(m.m06 == 0.0f);
    assert(m.m07 == 1.0f);
    assert(m.m08 == 0.0f);
}

int main() {
    test_matrix3x3_constants();
    test_matrix3x3_diagonal();
    test_matrix3x3_inertia_tensor();
    test_matrix3x3_scalar_multiplication();
    test_matrix3x3_addition();
    test_matrix3x3_transpose();
    test_matrix3x3_determinant();
    test_matrix3x3_inverse();
    test_matrix3x3_cross_product_matrix();
    
    return 0;
}
