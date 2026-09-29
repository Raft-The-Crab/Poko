/**
 * @file test_runtime.cpp
 * @brief Unit tests for core runtime object system
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"
#include "core/handles/handle_manager.h"
#include <cassert>
#include <iostream>
#include <string>

namespace poko {
namespace core {
namespace runtime {

bool test_instance_construction() {
    std::cout << "Testing Instance construction..." << std::endl;
    
    // Construction with type and name
    Instance instance1(1, "TestObject");
    assert(instance1.getName() == "TestObject");
    assert(instance1.getType() == 1);
    assert(instance1.getState() == InstanceState::Created);
    
    // Construction with different type
    Instance instance2(2, "AnotherObject");
    assert(instance2.getName() == "AnotherObject");
    assert(instance2.getType() == 2);
    
    // IDs should be unique
    assert(instance1.getId() != instance2.getId());
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_naming() {
    std::cout << "Testing Instance naming..." << std::endl;
    
    Instance instance(1, "OriginalName");
    
    // Set name
    instance.setName("NewName");
    assert(instance.getName() == "NewName");
    
    // Set empty name
    instance.setName("");
    assert(instance.getName().empty());
    
    // Rename
    instance.setName("Renamed");
    assert(instance.getName() == "Renamed");
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_lifecycle() {
    std::cout << "Testing Instance lifecycle..." << std::endl;
    
    Instance instance(1, "TestObject");
    
    // Initial state
    assert(instance.getState() == InstanceState::Created);
    
    // Set to initializing
    instance.setState(InstanceState::Initializing);
    assert(instance.getState() == InstanceState::Initializing);
    
    // Set to active
    instance.setState(InstanceState::Active);
    assert(instance.getState() == InstanceState::Active);
    
    // Set to destroyed
    instance.setState(InstanceState::Destroyed);
    assert(instance.getState() == InstanceState::Destroyed);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_parent_child() {
    std::cout << "Testing Instance parent-child relationships..." << std::endl;
    
    Instance parent(1, "Parent");
    Instance child1(2, "Child1");
    Instance child2(3, "Child2");
    
    // Initial state - no parent or children
    assert(parent.getChildren().empty());
    assert(child1.getParent().isNull());
    
    // Add child
    parent.addChild(child1.getId());
    assert(parent.getChildren().size() == 1);
    assert(parent.getChildren()[0] == child1.getId());
    
    // Set child's parent
    child1.setParent(parent.getId());
    assert(child1.getParent().index == parent.getId().index);
    
    // Add another child
    parent.addChild(child2.getId());
    assert(parent.getChildren().size() == 2);
    
    // Remove child
    parent.removeChild(child1.getId());
    assert(parent.getChildren().size() == 1);
    
    // Remove non-existent child
    parent.removeChild(child1.getId());
    assert(parent.getChildren().size() == 1);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_properties() {
    std::cout << "Testing Instance properties..." << std::endl;
    
    Instance instance(1, "TestObject");
    
    // Set property
    instance.setProperty("health", "100");
    assert(instance.getProperty("health") == "100");
    assert(instance.hasProperty("health"));
    
    // Set another property
    instance.setProperty("name", "Player");
    assert(instance.getProperty("name") == "Player");
    
    // Overwrite property
    instance.setProperty("health", "200");
    assert(instance.getProperty("health") == "200");
    
    // Get non-existent property
    assert(instance.getProperty("nonexistent").empty());
    assert(!instance.hasProperty("nonexistent"));
    
    // Remove property
    instance.removeProperty("health");
    assert(instance.getProperty("health").empty());
    assert(!instance.hasProperty("health"));
    
    // Remove non-existent property
    instance.removeProperty("nonexistent");
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_tags() {
    std::cout << "Testing Instance tags..." << std::endl;
    
    Instance instance(1, "TestObject");
    
    // Add tag
    instance.addTag("player");
    assert(instance.hasTag("player"));
    assert(instance.getTags().size() == 1);
    
    // Add multiple tags
    instance.addTag("friendly");
    instance.addTag("ally");
    assert(instance.hasTag("player"));
    assert(instance.hasTag("friendly"));
    assert(instance.hasTag("ally"));
    assert(instance.getTags().size() == 3);
    
    // Add duplicate tag (should not duplicate)
    instance.addTag("player");
    assert(instance.getTags().size() == 3);
    
    // Remove tag
    instance.removeTag("player");
    assert(!instance.hasTag("player"));
    assert(instance.getTags().size() == 2);
    
    // Remove non-existent tag
    instance.removeTag("nonexistent");
    assert(instance.getTags().size() == 2);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_factory() {
    std::cout << "Testing Instance factory..." << std::endl;
    
    // Create instance using factory
    auto instance = createInstance(1, "FactoryObject");
    assert(instance != nullptr);
    assert(instance->getName() == "FactoryObject");
    assert(instance->getType() == 1);
    
    // Create another instance
    auto instance2 = createInstance(2, "AnotherFactoryObject");
    assert(instance2 != nullptr);
    assert(instance2->getId() != instance->getId());
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool test_instance_id_stability() {
    std::cout << "Testing Instance ID stability..." << std::endl;
    
    Instance instance(1, "TestObject");
    
    handles::Handle originalId = instance.getId();
    
    // Change name and state
    instance.setName("Renamed");
    instance.setState(InstanceState::Active);
    
    // ID should remain stable
    assert(instance.getId() == originalId);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

} // namespace runtime
} // namespace core
} // namespace poko

int main() {
    std::cout << "=== Core Runtime Tests ===" << std::endl;
    
    using namespace poko::core::runtime;
    
    bool allPassed = true;
    
    allPassed &= test_instance_construction();
    allPassed &= test_instance_naming();
    allPassed &= test_instance_lifecycle();
    allPassed &= test_instance_parent_child();
    allPassed &= test_instance_properties();
    allPassed &= test_instance_tags();
    allPassed &= test_instance_factory();
    allPassed &= test_instance_id_stability();
    
    if (allPassed) {
        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== Some tests failed! ===" << std::endl;
        return 1;
    }
}
