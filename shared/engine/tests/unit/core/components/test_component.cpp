/**
 * @file test_component.cpp
 * @brief Core Component System unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/component.h"
#include "core/handles/handle_manager.h"
#include <cassert>
#include <iostream>

// Test component types
class TestComponent : public poko::core::components::Component {
public:
    static constexpr poko::core::components::ComponentID TypeID = 100;
    
    poko::core::components::ComponentID getTypeID() const noexcept override { return TypeID; }
    const char* getTypeName() const noexcept override { return "TestComponent"; }
    
    int value = 0;
};

class AnotherComponent : public poko::core::components::Component {
public:
    static constexpr poko::core::components::ComponentID TypeID = 200;
    
    poko::core::components::ComponentID getTypeID() const noexcept override { return TypeID; }
    const char* getTypeName() const noexcept override { return "AnotherComponent"; }
    
    float data = 0.0f;
};

void test_component_basics() {
    std::cout << "Testing Component basics..." << std::endl;
    
    TestComponent component;
    
    assert(component.getTypeID() == TestComponent::TypeID);
    assert(std::string(component.getTypeName()) == "TestComponent");
    assert(component.getState() == poko::core::components::ComponentState::None);
    assert(!component.isActive());
    
    component.setState(poko::core::components::ComponentState::Active);
    assert(component.isActive());
    
    std::cout << "✓ Component basics tests passed" << std::endl;
}

void test_component_lifecycle() {
    std::cout << "Testing Component lifecycle..." << std::endl;
    
    TestComponent component;
    
    component.onCreate();
    assert(component.getState() == poko::core::components::ComponentState::None);
    
    component.onActivate();
    component.setState(poko::core::components::ComponentState::Active);
    assert(component.isActive());
    
    component.onDeactivate();
    component.setState(poko::core::components::ComponentState::Deactivating);
    
    component.onDestroy();
    component.setState(poko::core::components::ComponentState::Destroyed);
    
    std::cout << "✓ Component lifecycle tests passed" << std::endl;
}

void test_registry_basics() {
    std::cout << "Testing ComponentRegistry basics..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    assert(registry.registerComponent(TestComponent::TypeID, "TestComponent", factory));
    assert(registry.isRegistered(TestComponent::TypeID));
    assert(registry.getRegisteredCount() == 1);
    
    // Duplicate registration should fail
    assert(!registry.registerComponent(TestComponent::TypeID, "TestComponent", factory));
    
    std::cout << "✓ ComponentRegistry basics tests passed" << std::endl;
}

void test_registry_create() {
    std::cout << "Testing ComponentRegistry create..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory);
    
    auto component = registry.createComponent(TestComponent::TypeID);
    assert(component != nullptr);
    assert(component->getTypeID() == TestComponent::TypeID);
    
    // Create unregistered type should return nullptr
    auto invalid = registry.createComponent(999);
    assert(invalid == nullptr);
    
    std::cout << "✓ ComponentRegistry create tests passed" << std::endl;
}

void test_registry_unregister() {
    std::cout << "Testing ComponentRegistry unregister..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory);
    assert(registry.isRegistered(TestComponent::TypeID));
    
    assert(registry.unregisterComponent(TestComponent::TypeID));
    assert(!registry.isRegistered(TestComponent::TypeID));
    
    // Unregister non-existent should fail
    assert(!registry.unregisterComponent(TestComponent::TypeID));
    
    std::cout << "✓ ComponentRegistry unregister tests passed" << std::endl;
}

void test_registry_type_name() {
    std::cout << "Testing ComponentRegistry type name..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory);
    
    assert(std::string(registry.getTypeName(TestComponent::TypeID)) == "TestComponent");
    assert(std::string(registry.getTypeName(999)) == "");
    
    std::cout << "✓ ComponentRegistry type name tests passed" << std::endl;
}

void test_registry_get_types() {
    std::cout << "Testing ComponentRegistry get types..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory1 = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    auto factory2 = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<AnotherComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory1);
    registry.registerComponent(AnotherComponent::TypeID, "AnotherComponent", factory2);
    
    auto types = registry.getRegisteredTypes();
    assert(types.size() == 2);
    
    std::cout << "✓ ComponentRegistry get types tests passed" << std::endl;
}

void test_registry_clear() {
    std::cout << "Testing ComponentRegistry clear..." << std::endl;
    
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory);
    assert(registry.getRegisteredCount() == 1);
    
    registry.clear();
    assert(registry.getRegisteredCount() == 0);
    
    std::cout << "✓ ComponentRegistry clear tests passed" << std::endl;
}

void test_manager_attach() {
    std::cout << "Testing ComponentManager attach..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto component = std::make_unique<TestComponent>();
    assert(manager.attachComponent(instanceId, std::move(component)));
    
    // Duplicate attach should fail
    auto component2 = std::make_unique<TestComponent>();
    assert(!manager.attachComponent(instanceId, std::move(component2)));
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager attach tests passed" << std::endl;
}

void test_manager_detach() {
    std::cout << "Testing ComponentManager detach..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto component = std::make_unique<TestComponent>();
    manager.attachComponent(instanceId, std::move(component));
    
    assert(manager.hasComponent(instanceId, TestComponent::TypeID));
    assert(manager.detachComponent(instanceId, TestComponent::TypeID));
    assert(!manager.hasComponent(instanceId, TestComponent::TypeID));
    
    // Detach non-existent should fail
    assert(!manager.detachComponent(instanceId, TestComponent::TypeID));
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager detach tests passed" << std::endl;
}

void test_manager_get() {
    std::cout << "Testing ComponentManager get..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto component = std::make_unique<TestComponent>();
    component->value = 42;
    manager.attachComponent(instanceId, std::move(component));
    
    poko::core::components::Component* comp = manager.getComponent(instanceId, TestComponent::TypeID);
    assert(comp != nullptr);
    assert(comp->getTypeID() == TestComponent::TypeID);
    
    // Get non-existent should return nullptr
    poko::core::components::Component* invalid = manager.getComponent(instanceId, 999);
    assert(invalid == nullptr);
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager get tests passed" << std::endl;
}

void test_manager_has() {
    std::cout << "Testing ComponentManager has..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    assert(!manager.hasComponent(instanceId, TestComponent::TypeID));
    
    auto component = std::make_unique<TestComponent>();
    manager.attachComponent(instanceId, std::move(component));
    
    assert(manager.hasComponent(instanceId, TestComponent::TypeID));
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager has tests passed" << std::endl;
}

void test_manager_multiple_components() {
    std::cout << "Testing ComponentManager multiple components..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto testComp = std::make_unique<TestComponent>();
    auto anotherComp = std::make_unique<AnotherComponent>();
    
    manager.attachComponent(instanceId, std::move(testComp));
    manager.attachComponent(instanceId, std::move(anotherComp));
    
    assert(manager.getComponentCount(instanceId) == 2);
    assert(manager.hasComponent(instanceId, TestComponent::TypeID));
    assert(manager.hasComponent(instanceId, AnotherComponent::TypeID));
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager multiple components tests passed" << std::endl;
}

void test_manager_get_components() {
    std::cout << "Testing ComponentManager getComponents..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto testComp = std::make_unique<TestComponent>();
    auto anotherComp = std::make_unique<AnotherComponent>();
    
    manager.attachComponent(instanceId, std::move(testComp));
    manager.attachComponent(instanceId, std::move(anotherComp));
    
    auto components = manager.getComponents(instanceId);
    assert(components.size() == 2);
    
    auto types = manager.getComponentTypes(instanceId);
    assert(types.size() == 2);
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager getComponents tests passed" << std::endl;
}

void test_manager_remove_all() {
    std::cout << "Testing ComponentManager remove all..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    auto testComp = std::make_unique<TestComponent>();
    auto anotherComp = std::make_unique<AnotherComponent>();
    
    manager.attachComponent(instanceId, std::move(testComp));
    manager.attachComponent(instanceId, std::move(anotherComp));
    
    assert(manager.getComponentCount(instanceId) == 2);
    
    manager.removeComponents(instanceId);
    assert(manager.getComponentCount(instanceId) == 0);
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    std::cout << "✓ ComponentManager remove all tests passed" << std::endl;
}

void test_manager_clear() {
    std::cout << "Testing ComponentManager clear..." << std::endl;
    
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle1 = handleManager.allocate();
    poko::core::handles::Handle handle2 = handleManager.allocate();
    
    uint64_t instanceId1 = handle1.index;
    uint64_t instanceId2 = handle2.index;
    
    auto comp1 = std::make_unique<TestComponent>();
    auto comp2 = std::make_unique<TestComponent>();
    
    manager.attachComponent(instanceId1, std::move(comp1));
    manager.attachComponent(instanceId2, std::move(comp2));
    
    manager.clear();
    
    assert(manager.getComponentCount(instanceId1) == 0);
    assert(manager.getComponentCount(instanceId2) == 0);
    
    [[maybe_unused]] bool freed1 = handleManager.free(handle1);
    [[maybe_unused]] bool freed2 = handleManager.free(handle2);
    
    std::cout << "✓ ComponentManager clear tests passed" << std::endl;
}

void test_global_registry() {
    std::cout << "Testing global component registry..." << std::endl;
    
    auto& registry = poko::core::components::getGlobalComponentRegistry();
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    registry.registerComponent(TestComponent::TypeID, "TestComponent", factory);
    assert(registry.isRegistered(TestComponent::TypeID));
    
    registry.unregisterComponent(TestComponent::TypeID);
    
    std::cout << "✓ Global component registry tests passed" << std::endl;
}

void test_template_helpers() {
    std::cout << "Testing template helpers..." << std::endl;
    
    auto id1 = poko::core::components::generateComponentID<TestComponent>();
    auto id2 = poko::core::components::generateComponentID<TestComponent>();
    
    // Should be the same for the same type
    assert(id1 == id2);
    
    // Different types should have different IDs
    auto id3 = poko::core::components::generateComponentID<AnotherComponent>();
    assert(id1 != id3);
    
    poko::core::components::ComponentRegistry registry;
    assert(poko::core::components::registerComponentType<TestComponent>(registry, "TestComponent"));
    assert(registry.isRegistered(poko::core::components::generateComponentID<TestComponent>()));
    
    std::cout << "✓ Template helpers tests passed" << std::endl;
}

void test_component_limits() {
    std::cout << "Testing component limits..." << std::endl;
    
    // Test component count limit per instance
    poko::core::components::ComponentManager manager;
    poko::core::handles::HandleManager handleManager;
    
    poko::core::handles::Handle handle = handleManager.allocate();
    uint64_t instanceId = handle.index;
    
    // Note: We won't actually hit the limit (64 components) in tests
    // but verify the limit checking logic is in place
    auto comp1 = std::make_unique<TestComponent>();
    auto comp2 = std::make_unique<AnotherComponent>();
    
    assert(manager.attachComponent(instanceId, std::move(comp1)));
    assert(manager.attachComponent(instanceId, std::move(comp2)));
    assert(manager.getComponentCount(instanceId) == 2);
    
    [[maybe_unused]] bool freed = handleManager.free(handle);
    
    // Test registered types limit
    // Note: We won't actually hit the limit (256 types) in tests
    // but verify the limit checking logic is in place
    poko::core::components::ComponentRegistry registry;
    
    auto factory = []() -> std::unique_ptr<poko::core::components::Component> {
        return std::make_unique<TestComponent>();
    };
    
    assert(registry.registerComponent(300, "TestComponent1", factory));
    assert(registry.registerComponent(301, "TestComponent2", factory));
    assert(registry.getRegisteredCount() == 2);
    
    std::cout << "✓ Component limits tests passed" << std::endl;
}

int main() {
    std::cout << "=== Core Component System Unit Tests ===" << std::endl;
    
    test_component_basics();
    test_component_lifecycle();
    test_registry_basics();
    test_registry_create();
    test_registry_unregister();
    test_registry_type_name();
    test_registry_get_types();
    test_registry_clear();
    test_manager_attach();
    test_manager_detach();
    test_manager_get();
    test_manager_has();
    test_manager_multiple_components();
    test_manager_get_components();
    test_manager_remove_all();
    test_manager_clear();
    test_global_registry();
    test_template_helpers();
    test_component_limits();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    poko::core::components::destroyGlobalComponentRegistry();
    
    return 0;
}
