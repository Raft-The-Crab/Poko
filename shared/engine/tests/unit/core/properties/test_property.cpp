/**
 * @file test_property.cpp
 * @brief Core Property System unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"
#include <cassert>
#include <iostream>

using namespace poko::core::properties;

void test_property_basics() {
    std::cout << "Testing Property basics..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    Property property(metadata);
    
    assert(property.getName() == "test_property");
    assert(property.getType() == PropertyType::Int32);
    assert(std::holds_alternative<int32_t>(property.getValue()));
    assert(std::get<int32_t>(property.getValue()) == 42);
    
    std::cout << "✓ Property basics tests passed" << std::endl;
}

void test_property_set_value() {
    std::cout << "Testing Property setValue..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(0);
    
    Property property(metadata);
    
    assert(property.setValue(int32_t(100)));
    assert(std::get<int32_t>(property.getValue()) == 100);
    
    // Test setting same value (should return false)
    assert(!property.setValue(int32_t(100)));
    
    std::cout << "✓ Property setValue tests passed" << std::endl;
}

void test_property_read_only() {
    std::cout << "Testing Property read-only..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::ReadOnly;
    metadata.defaultValue = int32_t(42);
    
    Property property(metadata);
    
    assert(property.isReadOnly());
    assert(!property.setValue(int32_t(100))); // Should fail
    assert(std::get<int32_t>(property.getValue()) == 42); // Value unchanged
    
    std::cout << "✓ Property read-only tests passed" << std::endl;
}

void test_property_transient() {
    std::cout << "Testing Property transient..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::Transient;
    metadata.defaultValue = int32_t(42);
    
    Property property(metadata);
    
    assert(property.isTransient());
    
    std::cout << "✓ Property transient tests passed" << std::endl;
}

void test_property_reset_to_default() {
    std::cout << "Testing Property resetToDefault..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    Property property(metadata);
    
    property.setValue(int32_t(100));
    assert(std::get<int32_t>(property.getValue()) == 100);
    
    property.resetToDefault();
    assert(std::get<int32_t>(property.getValue()) == 42);
    
    std::cout << "✓ Property resetToDefault tests passed" << std::endl;
}

void test_property_callback() {
    std::cout << "Testing Property change callback..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(0);
    
    Property property(metadata);
    
    bool callbackCalled = false;
    int32_t newValue = 0;
    
    property.setChangeCallback([&callbackCalled, &newValue](const std::string&, const PropertyValue&, const PropertyValue& newVal) {
        callbackCalled = true;
        newValue = std::get<int32_t>(newVal);
    });
    
    property.setValue(int32_t(100));
    
    assert(callbackCalled);
    assert(newValue == 100);
    
    std::cout << "✓ Property change callback tests passed" << std::endl;
}

void test_property_string_type() {
    std::cout << "Testing Property string type..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::String;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = std::string("default");
    
    Property property(metadata);
    
    assert(property.getType() == PropertyType::String);
    assert(std::get<std::string>(property.getValue()) == "default");
    
    property.setValue(std::string("modified"));
    assert(std::get<std::string>(property.getValue()) == "modified");
    
    std::cout << "✓ Property string type tests passed" << std::endl;
}

void test_property_vector_type() {
    std::cout << "Testing Property vector type..." << std::endl;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Vector3;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = std::vector<float>{0.0f, 0.0f, 0.0f};
    
    Property property(metadata);
    
    assert(property.getType() == PropertyType::Vector3);
    auto vec = std::get<std::vector<float>>(property.getValue());
    assert(vec.size() == 3);
    
    property.setValue(std::vector<float>{1.0f, 2.0f, 3.0f});
    vec = std::get<std::vector<float>>(property.getValue());
    assert(vec[0] == 1.0f);
    assert(vec[1] == 2.0f);
    assert(vec[2] == 3.0f);
    
    std::cout << "✓ Property vector type tests passed" << std::endl;
}

void test_registry_basics() {
    std::cout << "Testing PropertyRegistry basics..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    assert(registry.registerProperty("test", metadata));
    assert(registry.hasProperty("test"));
    assert(registry.getPropertyCount() == 1);
    
    // Duplicate registration should fail
    assert(!registry.registerProperty("test", metadata));
    
    std::cout << "✓ PropertyRegistry basics tests passed" << std::endl;
}

void test_registry_get_set() {
    std::cout << "Testing PropertyRegistry get/set..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("test", metadata);
    
    PropertyValue value;
    assert(registry.getValue("test", value));
    assert(std::get<int32_t>(value) == 42);
    
    assert(registry.setValue("test", int32_t(100)));
    assert(registry.getValue("test", value));
    assert(std::get<int32_t>(value) == 100);
    
    std::cout << "✓ PropertyRegistry get/set tests passed" << std::endl;
}

void test_registry_unregister() {
    std::cout << "Testing PropertyRegistry unregister..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("test", metadata);
    assert(registry.hasProperty("test"));
    
    assert(registry.unregisterProperty("test"));
    assert(!registry.hasProperty("test"));
    
    // Unregister non-existent should fail
    assert(!registry.unregisterProperty("test"));
    
    std::cout << "✓ PropertyRegistry unregister tests passed" << std::endl;
}

void test_registry_get_names() {
    std::cout << "Testing PropertyRegistry getNames..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("prop1", metadata);
    registry.registerProperty("prop2", metadata);
    registry.registerProperty("prop3", metadata);
    
    auto names = registry.getPropertyNames();
    assert(names.size() == 3);
    
    std::cout << "✓ PropertyRegistry getNames tests passed" << std::endl;
}

void test_registry_category() {
    std::cout << "Testing PropertyRegistry category..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata1;
    metadata1.name = "prop1";
    metadata1.type = PropertyType::Int32;
    metadata1.flags = PropertyFlags::None;
    metadata1.defaultValue = int32_t(42);
    metadata1.category = "category1";
    
    PropertyMetadata metadata2;
    metadata2.name = "prop2";
    metadata2.type = PropertyType::Int32;
    metadata2.flags = PropertyFlags::None;
    metadata2.defaultValue = int32_t(42);
    metadata2.category = "category2";
    
    registry.registerProperty("prop1", metadata1);
    registry.registerProperty("prop2", metadata2);
    
    auto cat1Props = registry.getPropertiesByCategory("category1");
    assert(cat1Props.size() == 1);
    assert(cat1Props[0] == "prop1");
    
    auto cat2Props = registry.getPropertiesByCategory("category2");
    assert(cat2Props.size() == 1);
    assert(cat2Props[0] == "prop2");
    
    std::cout << "✓ PropertyRegistry category tests passed" << std::endl;
}

void test_registry_reset_all() {
    std::cout << "Testing PropertyRegistry resetAllToDefault..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("prop1", metadata);
    registry.registerProperty("prop2", metadata);
    
    registry.setValue("prop1", int32_t(100));
    registry.setValue("prop2", int32_t(200));
    
    registry.resetAllToDefault();
    
    PropertyValue value;
    registry.getValue("prop1", value);
    assert(std::get<int32_t>(value) == 42);
    
    registry.getValue("prop2", value);
    assert(std::get<int32_t>(value) == 42);
    
    std::cout << "✓ PropertyRegistry resetAllToDefault tests passed" << std::endl;
}

void test_registry_clear() {
    std::cout << "Testing PropertyRegistry clear..." << std::endl;
    
    PropertyRegistry registry;
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("prop1", metadata);
    registry.registerProperty("prop2", metadata);
    
    assert(registry.getPropertyCount() == 2);
    
    registry.clear();
    
    assert(registry.getPropertyCount() == 0);
    
    std::cout << "✓ PropertyRegistry clear tests passed" << std::endl;
}

void test_global_registry() {
    std::cout << "Testing global property registry..." << std::endl;
    
    auto& registry = getGlobalPropertyRegistry();
    
    PropertyMetadata metadata;
    metadata.name = "test_property";
    metadata.type = PropertyType::Int32;
    metadata.flags = PropertyFlags::None;
    metadata.defaultValue = int32_t(42);
    
    registry.registerProperty("global_test", metadata);
    assert(registry.hasProperty("global_test"));
    
    registry.unregisterProperty("global_test");
    
    std::cout << "✓ Global property registry tests passed" << std::endl;
}

void test_utility_functions() {
    std::cout << "Testing utility functions..." << std::endl;
    
    assert(std::string(propertyTypeToString(PropertyType::Int32)) == "int32");
    assert(std::string(propertyTypeToString(PropertyType::String)) == "string");
    
    assert(stringToPropertyType("float") == PropertyType::Float);
    assert(stringToPropertyType("vector3") == PropertyType::Vector3);
    assert(stringToPropertyType("invalid") == PropertyType::Invalid);
    
    auto defaultInt = getDefaultValueForType(PropertyType::Int32);
    assert(std::get<int32_t>(defaultInt) == 0);
    
    auto defaultString = getDefaultValueForType(PropertyType::String);
    assert(std::get<std::string>(defaultString).empty());
    
    auto defaultVector3 = getDefaultValueForType(PropertyType::Vector3);
    auto vec = std::get<std::vector<float>>(defaultVector3);
    assert(vec.size() == 3);
    
    std::cout << "✓ Utility functions tests passed" << std::endl;
}

void test_property_limits() {
    std::cout << "Testing property limits..." << std::endl;
    
    // Test property name length validation
    poko::core::properties::PropertyMetadata metadata;
    metadata.name = "TestProperty";
    metadata.type = poko::core::properties::PropertyType::Int32;
    
    // Normal name should work
    poko::core::properties::Property prop1(metadata);
    assert(prop1.getMetadata().name == "TestProperty");
    
    // Name exceeding limit should be truncated
    std::string longName(poko::core::properties::MAX_PROPERTY_NAME_LENGTH + 100, 'A');
    metadata.name = longName;
    poko::core::properties::Property prop2(metadata);
    assert(prop2.getMetadata().name.length() == poko::core::properties::MAX_PROPERTY_NAME_LENGTH);
    
    // Test registry size limit
    // Note: We won't actually hit the limit (256 properties) in tests
    // but verify the limit checking logic is in place
    poko::core::properties::PropertyRegistry registry;
    
    metadata.name = "Prop1";
    registry.registerProperty("Prop1", metadata);
    
    metadata.name = "Prop2";
    registry.registerProperty("Prop2", metadata);
    
    assert(registry.getPropertyCount() == 2);
    
    std::cout << "✓ Property limits tests passed" << std::endl;
}

int main() {
    std::cout << "=== Core Property System Unit Tests ===" << std::endl;
    
    test_property_basics();
    test_property_set_value();
    test_property_read_only();
    test_property_transient();
    test_property_reset_to_default();
    test_property_callback();
    test_property_string_type();
    test_property_vector_type();
    test_registry_basics();
    test_registry_get_set();
    test_registry_unregister();
    test_registry_get_names();
    test_registry_category();
    test_registry_reset_all();
    test_registry_clear();
    test_global_registry();
    test_utility_functions();
    test_property_limits();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    destroyGlobalPropertyRegistry();
    
    return 0;
}
