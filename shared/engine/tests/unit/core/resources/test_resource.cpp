/**
 * @file test_resource.cpp
 * @brief Core Resource Management unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/resources/resource.h"
#include <cassert>
#include <iostream>

using namespace poko::core::resources;

// Test resource class
class TestResourceData {
public:
    int value;
    std::string content;
    
    TestResourceData() : value(0) {}
    TestResourceData(int v, const std::string& c) : value(v), content(c) {}
};

class TestResource : public Resource<TestResourceData> {
public:
    TestResource(const std::string& name, ResourceType type)
        : Resource<TestResourceData>(name, type)
    {}
    
    bool load() override {
        setData(std::make_unique<TestResourceData>(42, "Test content"));
        setState(ResourceState::Loaded);
        return true;
    }
    
    void unload() override {
        Resource<TestResourceData>::unload();
    }
};

void test_resource_basics() {
    std::cout << "Testing Resource basics..." << std::endl;
    
    TestResource resource("TestResource", ResourceType::Data);
    
    assert(resource.getName() == "TestResource");
    assert(resource.getType() == ResourceType::Data);
    assert(resource.getState() == ResourceState::Unloaded);
    assert(!resource.isLoaded());
    assert(!resource.isLoading());
    assert(!resource.isFailed());
    
    std::cout << "✓ Resource basics tests passed" << std::endl;
}

void test_resource_lifecycle() {
    std::cout << "Testing Resource lifecycle..." << std::endl;
    
    TestResource resource("TestResource", ResourceType::Data);
    
    assert(resource.getState() == ResourceState::Unloaded);
    
    resource.setState(ResourceState::Loading);
    assert(resource.isLoading());
    
    resource.setState(ResourceState::Loaded);
    assert(resource.isLoaded());
    
    resource.setState(ResourceState::Failed);
    assert(resource.isFailed());
    
    std::cout << "✓ Resource lifecycle tests passed" << std::endl;
}

void test_resource_data() {
    std::cout << "Testing Resource data..." << std::endl;
    
    TestResource resource("TestResource", ResourceType::Data);
    
    assert(resource.getData() == nullptr);
    
    resource.load();
    assert(resource.getData() != nullptr);
    assert(resource.getData()->value == 42);
    assert(resource.getData()->content == "Test content");
    
    resource.unload();
    assert(resource.getData() == nullptr);
    
    std::cout << "✓ Resource data tests passed" << std::endl;
}

void test_resource_reference_count() {
    std::cout << "Testing Resource reference count..." << std::endl;
    
    TestResource resource("TestResource", ResourceType::Data);
    
    assert(resource.getRefCount() == 0);
    
    resource.incrementRefCount();
    assert(resource.getRefCount() == 1);
    
    resource.incrementRefCount();
    assert(resource.getRefCount() == 2);
    
    resource.decrementRefCount();
    assert(resource.getRefCount() == 1);
    
    resource.decrementRefCount();
    assert(resource.getRefCount() == 0);
    
    std::cout << "✓ Resource reference count tests passed" << std::endl;
}

void test_resource_handle() {
    std::cout << "Testing Resource handle..." << std::endl;
    
    ResourceHandle handle1;
    assert(!handle1.isValid());
    
    ResourceHandle handle2(100, 5);
    assert(handle2.isValid());
    assert(handle2.index == 100);
    assert(handle2.generation == 5);
    
    ResourceHandle handle3(100, 6);
    assert(handle3.isValid());
    assert(handle3.index == 100);
    assert(handle3.generation == 6);
    
    std::cout << "✓ Resource handle tests passed" << std::endl;
}

void test_manager_basics() {
    std::cout << "Testing ResourceManager basics..." << std::endl;
    
    ResourceManager manager;
    
    assert(manager.getResourceCount() == 0);
    assert(manager.getTotalMemoryUsage() == 0);
    
    manager.clear();
    assert(manager.getResourceCount() == 0);
    
    std::cout << "✓ ResourceManager basics tests passed" << std::endl;
}

void test_manager_loader_registration() {
    std::cout << "Testing ResourceManager loader registration..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& /*filepath*/, ResourceType type) -> ResourceBase::Ptr {
        return std::make_shared<TestResource>("Test", type);
    };
    
    assert(manager.registerLoader(ResourceType::Data, loader));
    assert(!manager.registerLoader(ResourceType::Data, loader)); // Duplicate
    
    assert(manager.unregisterLoader(ResourceType::Data));
    assert(!manager.unregisterLoader(ResourceType::Data)); // Already unregistered
    
    assert(!manager.registerLoader(ResourceType::Unknown, loader)); // Invalid type
    
    std::cout << "✓ ResourceManager loader registration tests passed" << std::endl;
}

void test_manager_load_resource() {
    std::cout << "Testing ResourceManager load resource..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("TestResource", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("test.data", ResourceType::Data);
    assert(handle.isValid());
    
    ResourceBase* resource = manager.getResource(handle);
    assert(resource != nullptr);
    assert(resource->getName() == "TestResource");
    assert(resource->getType() == ResourceType::Data);
    assert(resource->isLoaded());
    
    std::cout << "✓ ResourceManager load resource tests passed" << std::endl;
}

void test_manager_unload_resource() {
    std::cout << "Testing ResourceManager unload resource..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("TestResource", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("test.data", ResourceType::Data);
    assert(handle.isValid());
    assert(manager.hasResource(handle));
    
    assert(manager.unloadResource(handle));
    assert(!manager.hasResource(handle));
    assert(manager.getResource(handle) == nullptr);
    
    std::cout << "✓ ResourceManager unload resource tests passed" << std::endl;
}

void test_manager_get_by_name() {
    std::cout << "Testing ResourceManager get by name..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("CustomName", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("test.data", "CustomName", ResourceType::Data);
    assert(handle.isValid());
    
    ResourceBase* resource = manager.getResource("CustomName");
    assert(resource != nullptr);
    assert(resource->getName() == "CustomName");
    
    assert(manager.hasResource("CustomName"));
    
    std::cout << "✓ ResourceManager get by name tests passed" << std::endl;
}

void test_manager_get_all_resources() {
    std::cout << "Testing ResourceManager get all resources..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>(filepath, type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    manager.loadResource("test1.data", ResourceType::Data);
    manager.loadResource("test2.data", ResourceType::Data);
    manager.loadResource("test3.data", ResourceType::Data);
    
    auto resources = manager.getAllResources();
    assert(resources.size() == 3);
    
    std::cout << "✓ ResourceManager get all resources tests passed" << std::endl;
}

void test_manager_get_by_type() {
    std::cout << "Testing ResourceManager get by type..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>(filepath, type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    manager.registerLoader(ResourceType::Texture, loader);
    
    manager.loadResource("test1.data", ResourceType::Data);
    manager.loadResource("test2.data", ResourceType::Data);
    manager.loadResource("test1.tex", ResourceType::Texture);
    
    auto dataResources = manager.getResourcesByType(ResourceType::Data);
    assert(dataResources.size() == 2);
    
    auto textureResources = manager.getResourcesByType(ResourceType::Texture);
    assert(textureResources.size() == 1);
    
    std::cout << "✓ ResourceManager get by type tests passed" << std::endl;
}

void test_manager_reload_resource() {
    std::cout << "Testing ResourceManager reload resource..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>(filepath, type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("test.data", ResourceType::Data);
    assert(handle.isValid());
    
    assert(manager.reloadResource(handle));
    assert(manager.getResourceState(handle) == ResourceState::Loaded);
    
    assert(manager.reloadResource("test"));
    assert(manager.getResourceState("test") == ResourceState::Loaded);
    
    std::cout << "✓ ResourceManager reload resource tests passed" << std::endl;
}

void test_manager_clear() {
    std::cout << "Testing ResourceManager clear..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>(filepath, type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    manager.loadResource("test1.data", ResourceType::Data);
    manager.loadResource("test2.data", ResourceType::Data);
    
    assert(manager.getResourceCount() == 2);
    
    manager.clear();
    
    assert(manager.getResourceCount() == 0);
    assert(manager.getTotalMemoryUsage() == 0);
    
    std::cout << "✓ ResourceManager clear tests passed" << std::endl;
}

void test_manager_resource_info() {
    std::cout << "Testing ResourceManager resource info..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("TestResource", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("test.data", ResourceType::Data);
    assert(handle.isValid());
    
    ResourceManager::ResourceInfo info;
    assert(manager.getResourceInfo(handle, info));
    assert(info.name == "TestResource");
    assert(info.filepath == "test.data");
    assert(info.type == ResourceType::Data);
    assert(info.state == ResourceState::Loaded);
    
    assert(manager.getResourceInfo("TestResource", info));
    assert(info.name == "TestResource");
    
    std::cout << "✓ ResourceManager resource info tests passed" << std::endl;
}

void test_global_registry() {
    std::cout << "Testing global resource manager..." << std::endl;
    
    auto& manager = getGlobalResourceManager();
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("GlobalTest", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    ResourceHandle handle = manager.loadResource("global.data", ResourceType::Data);
    assert(handle.isValid());
    
    manager.clear();
    
    std::cout << "✓ Global resource manager tests passed" << std::endl;
}

void test_resource_limits() {
    std::cout << "Testing resource limits..." << std::endl;
    
    ResourceManager manager;
    
    auto loader = [](const std::string& filepath, ResourceType type) -> ResourceBase::Ptr {
        auto resource = std::make_shared<TestResource>("Test", type);
        resource->setFilePath(filepath);
        resource->load();
        return resource;
    };
    
    manager.registerLoader(ResourceType::Data, loader);
    
    // Test empty filepath rejection
    ResourceHandle handle1 = manager.loadResource("", ResourceType::Data);
    assert(!handle1.isValid());
    
    // Test empty name rejection
    ResourceHandle handle2 = manager.loadResource("test.data", "", ResourceType::Data);
    assert(!handle2.isValid());
    
    // Test duplicate prevention
    ResourceHandle handle3 = manager.loadResource("test.data", "DuplicateTest", ResourceType::Data);
    assert(handle3.isValid());
    
    ResourceHandle handle4 = manager.loadResource("test.data", "DuplicateTest", ResourceType::Data);
    assert(handle4.isValid());
    assert(handle4.index == handle3.index); // Same resource
    
    std::cout << "✓ Resource limits tests passed" << std::endl;
}

int main() {
    std::cout << "=== Core Resource Management Unit Tests ===" << std::endl;
    
    test_resource_basics();
    test_resource_lifecycle();
    test_resource_data();
    test_resource_reference_count();
    test_resource_handle();
    test_manager_basics();
    test_manager_loader_registration();
    test_manager_load_resource();
    test_manager_unload_resource();
    test_manager_get_by_name();
    test_manager_get_all_resources();
    test_manager_get_by_type();
    test_manager_reload_resource();
    test_manager_clear();
    test_manager_resource_info();
    test_global_registry();
    test_resource_limits();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    destroyGlobalResourceManager();
    
    return 0;
}
