/**
 * @file test_transform.cpp
 * @brief Transform component unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/transform/transform.h"
#include <cassert>
#include <iostream>

using namespace poko::core::components::transform;

void test_vector3() {
    std::cout << "Testing Vector3..." << std::endl;
    
    // Default constructor
    Vector3 v1;
    assert(v1.x == 0.0f);
    assert(v1.y == 0.0f);
    assert(v1.z == 0.0f);
    
    // Component constructor
    Vector3 v2(1.0f, 2.0f, 3.0f);
    assert(v2.x == 1.0f);
    assert(v2.y == 2.0f);
    assert(v2.z == 3.0f);
    
    // Constants
    assert(Vector3::zero().x == 0.0f);
    assert(Vector3::one().x == 1.0f);
    assert(Vector3::up().y == 1.0f);
    assert(Vector3::down().y == -1.0f);
    assert(Vector3::forward().z == 1.0f);
    assert(Vector3::back().z == -1.0f);
    assert(Vector3::right().x == 1.0f);
    assert(Vector3::left().x == -1.0f);
    
    std::cout << "✓ Vector3 tests passed" << std::endl;
}

void test_quaternion() {
    std::cout << "Testing Quaternion..." << std::endl;
    
    // Default constructor
    Quaternion q1;
    assert(q1.w == 1.0f);
    assert(q1.x == 0.0f);
    assert(q1.y == 0.0f);
    assert(q1.z == 0.0f);
    
    // Component constructor
    Quaternion q2(0.5f, 0.5f, 0.5f, 0.5f);
    assert(q2.w == 0.5f);
    assert(q2.x == 0.5f);
    assert(q2.y == 0.5f);
    assert(q2.z == 0.5f);
    
    // Identity constant
    Quaternion identity = Quaternion::identity();
    assert(identity.w == 1.0f);
    assert(identity.x == 0.0f);
    assert(identity.y == 0.0f);
    assert(identity.z == 0.0f);
    
    std::cout << "✓ Quaternion tests passed" << std::endl;
}

void test_matrix4x4() {
    std::cout << "Testing Matrix4x4..." << std::endl;
    
    // Default constructor (identity)
    Matrix4x4 m1;
    assert(m1.data[0] == 1.0f);
    assert(m1.data[5] == 1.0f);
    assert(m1.data[10] == 1.0f);
    assert(m1.data[15] == 1.0f);
    
    // Identity constant
    Matrix4x4 identity = Matrix4x4::identity();
    assert(identity.data[0] == 1.0f);
    assert(identity.data[5] == 1.0f);
    assert(identity.data[10] == 1.0f);
    assert(identity.data[15] == 1.0f);
    
    std::cout << "✓ Matrix4x4 tests passed" << std::endl;
}

void test_transform_basics() {
    std::cout << "Testing Transform basics..." << std::endl;
    
    Transform transform;
    
    // Default values
    assert(transform.getLocalPosition().x == 0.0f);
    assert(transform.getLocalRotation().w == 1.0f);
    assert(transform.getLocalScale().x == 1.0f);
    
    // Set local position
    transform.setLocalPosition(Vector3(10.0f, 20.0f, 30.0f));
    assert(transform.getLocalPosition().x == 10.0f);
    assert(transform.getLocalPosition().y == 20.0f);
    assert(transform.getLocalPosition().z == 30.0f);
    
    // Set local rotation
    transform.setLocalRotation(Quaternion(0.707f, 0.0f, 0.707f, 0.0f));
    assert(transform.getLocalRotation().w == 0.707f);
    
    // Set local scale
    transform.setLocalScale(Vector3(2.0f, 3.0f, 4.0f));
    assert(transform.getLocalScale().x == 2.0f);
    assert(transform.getLocalScale().y == 3.0f);
    assert(transform.getLocalScale().z == 4.0f);
    
    std::cout << "✓ Transform basics tests passed" << std::endl;
}

void test_transform_world() {
    std::cout << "Testing Transform world transform..." << std::endl;
    
    Transform transform;
    
    // Set local transform
    transform.setLocalPosition(Vector3(5.0f, 10.0f, 15.0f));
    transform.setLocalScale(Vector3(2.0f, 2.0f, 2.0f));
    
    // Update world transform
    transform.updateWorldTransform();
    
    // Without parent, world should equal local
    assert(transform.getWorldPosition().x == 5.0f);
    assert(transform.getWorldPosition().y == 10.0f);
    assert(transform.getWorldPosition().z == 15.0f);
    assert(transform.getWorldScale().x == 2.0f);
    assert(transform.getWorldScale().y == 2.0f);
    assert(transform.getWorldScale().z == 2.0f);
    
    std::cout << "✓ Transform world transform tests passed" << std::endl;
}

void test_transform_parent() {
    std::cout << "Testing Transform parent hierarchy..." << std::endl;
    
    Transform parent;
    Transform child;
    
    // Set parent transform
    parent.setLocalPosition(Vector3(10.0f, 0.0f, 0.0f));
    parent.setLocalScale(Vector3(2.0f, 2.0f, 2.0f));
    parent.updateWorldTransform();
    
    // Set child transform
    child.setLocalPosition(Vector3(5.0f, 0.0f, 0.0f));
    child.setLocalScale(Vector3(3.0f, 3.0f, 3.0f));
    
    // Set parent
    child.setParent(&parent);
    
    // Update child world transform
    child.updateWorldTransform();
    
    // Child world position should be parent world + child local
    assert(child.getWorldPosition().x == 15.0f); // 10 + 5
    assert(child.getWorldPosition().y == 0.0f);
    assert(child.getWorldPosition().z == 0.0f);
    
    // Child world scale should be parent world * child local
    assert(child.getWorldScale().x == 6.0f); // 2 * 3
    assert(child.getWorldScale().y == 6.0f);
    assert(child.getWorldScale().z == 6.0f);
    
    // Check parent relationship
    assert(child.hasParent());
    assert(child.getParent() == &parent);
    
    // Remove parent
    child.setParent(nullptr);
    assert(!child.hasParent());
    assert(child.getParent() == nullptr);
    
    std::cout << "✓ Transform parent hierarchy tests passed" << std::endl;
}

void test_transform_dirty() {
    std::cout << "Testing Transform dirty flag..." << std::endl;
    
    Transform transform;
    
    // Initially dirty
    assert(transform.isDirty());
    
    // Update clears dirty flag
    transform.updateWorldTransform();
    assert(!transform.isDirty());
    
    // Modifying transform sets dirty flag
    transform.setLocalPosition(Vector3(1.0f, 0.0f, 0.0f));
    assert(transform.isDirty());
    
    transform.updateWorldTransform();
    assert(!transform.isDirty());
    
    transform.setLocalRotation(Quaternion::identity());
    assert(transform.isDirty());
    
    transform.updateWorldTransform();
    assert(!transform.isDirty());
    
    transform.setLocalScale(Vector3::one());
    assert(transform.isDirty());
    
    transform.updateWorldTransform();
    assert(!transform.isDirty());
    
    std::cout << "✓ Transform dirty flag tests passed" << std::endl;
}

void test_transform_matrices() {
    std::cout << "Testing Transform matrices..." << std::endl;
    
    Transform transform;
    
    transform.setLocalPosition(Vector3(10.0f, 20.0f, 30.0f));
    transform.setLocalScale(Vector3(2.0f, 3.0f, 4.0f));
    transform.updateWorldTransform();
    
    // Get matrices
    const Matrix4x4& localToWorld = transform.getLocalToWorldMatrix();
    (void)transform.getWorldToLocalMatrix(); // Suppress nodiscard warning
    
    // Check local-to-world matrix has translation
    assert(localToWorld.data[12] == 10.0f);
    assert(localToWorld.data[13] == 20.0f);
    assert(localToWorld.data[14] == 30.0f);
    
    // Check local-to-world matrix has scale
    assert(localToWorld.data[0] == 2.0f);
    assert(localToWorld.data[5] == 3.0f);
    assert(localToWorld.data[10] == 4.0f);
    
    std::cout << "✓ Transform matrices tests passed" << std::endl;
}

int main() {
    std::cout << "=== Transform Component Unit Tests ===" << std::endl;
    
    test_vector3();
    test_quaternion();
    test_matrix4x4();
    test_transform_basics();
    test_transform_world();
    test_transform_parent();
    test_transform_dirty();
    test_transform_matrices();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
