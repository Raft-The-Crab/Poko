/**
 * @file manager.cpp
 * @brief Resource manager implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/resources/resource.h"
#include <algorithm>

namespace poko {
namespace core {
namespace resources {

ResourceManager::ResourceManager()
    : m_nextIndex(1)
    , m_nextGeneration(1)
{
}

ResourceManager::~ResourceManager() {
    clear();
}

bool ResourceManager::registerLoader(ResourceType type, ResourceLoader loader) {
    // Validate type
    if (type == ResourceType::Unknown) {
        return false;
    }
    
    // Validate loader function
    if (!loader) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check for duplicate registration
    if (m_loaders.find(type) != m_loaders.end()) {
        return false; // Already registered
    }
    
    m_loaders[type] = loader;
    return true;
}

bool ResourceManager::unregisterLoader(ResourceType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_loaders.find(type);
    if (it == m_loaders.end()) {
        return false;
    }
    
    m_loaders.erase(it);
    return true;
}

ResourceHandle ResourceManager::loadResource(const std::string& filepath, ResourceType type) {
    // Validate filepath length
    if (filepath.empty() || filepath.length() > MAX_RESOURCE_FILEPATH_LENGTH) {
        return ResourceHandle();
    }
    
    // Extract name from filepath
    std::string name = filepath;
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        name = filepath.substr(lastSlash + 1);
    }
    
    // Remove extension
    size_t lastDot = name.find_last_of('.');
    if (lastDot != std::string::npos) {
        name = name.substr(0, lastDot);
    }
    
    // Validate extracted name length
    if (name.empty() || name.length() > MAX_RESOURCE_NAME_LENGTH) {
        return ResourceHandle();
    }
    
    return loadResource(filepath, name, type);
}

ResourceHandle ResourceManager::loadResource(const std::string& filepath, const std::string& name, ResourceType type) {
    // Validate filepath length
    if (filepath.empty() || filepath.length() > MAX_RESOURCE_FILEPATH_LENGTH) {
        return ResourceHandle();
    }
    
    // Validate name length
    if (name.empty() || name.length() > MAX_RESOURCE_NAME_LENGTH) {
        return ResourceHandle();
    }
    
    // Validate resource type
    if (type == ResourceType::Unknown) {
        return ResourceHandle();
    }
    
    // Check if resource already exists
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_resourcesByName.find(name) != m_resourcesByName.end()) {
            auto& existing = m_resourcesByName[name];
            return existing->getHandle();
        }
    }
    
    // Check resource count limit
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_resourcesByIndex.size() >= MAX_RESOURCES_LOADED) {
            return ResourceHandle();
        }
    }
    
    // Check total memory limit
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        size_t currentMemory = getTotalMemoryUsage();
        if (currentMemory >= MAX_RESOURCE_MEMORY_MB * 1024 * 1024) {
            return ResourceHandle();
        }
    }
    
    // Get loader for resource type
    ResourceLoader loader;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_loaders.find(type);
        if (it == m_loaders.end()) {
            return ResourceHandle(); // No loader registered for this type
        }
        loader = it->second;
    }
    
    // Validate loader function
    if (!loader) {
        return ResourceHandle();
    }
    
    // Load resource
    auto resource = loader(filepath, type);
    if (!resource) {
        return ResourceHandle();
    }
    
    // Set resource metadata
    resource->setFilePath(filepath);
    resource->setState(ResourceState::Loading);
    
    // Try to load resource data
    if (!resource->load()) {
        resource->setState(ResourceState::Failed);
        return ResourceHandle();
    }
    
    resource->setState(ResourceState::Loaded);
    
    // Assign handle
    uint64_t index = m_nextIndex++;
    uint32_t generation = m_nextGeneration++;
    ResourceHandle handle(index, generation);
    
    resource->setID(index);
    resource->setHandle(handle);
    
    // Add to registries
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_resourcesByIndex[index] = resource;
        m_resourcesByName[name] = resource;
        m_resourcesByGeneration[generation] = resource.get();
    }
    
    return handle;
}

bool ResourceManager::unloadResource(ResourceHandle handle) {
    // Validate handle
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return false; // Resource not found
    }
    
    auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return false; // Stale handle
    }
    
    // Unload resource data
    resource->unload();
    
    // Remove from registries
    m_resourcesByName.erase(resource->getName());
    m_resourcesByGeneration.erase(handle.generation);
    m_resourcesByIndex.erase(it);
    
    return true;
}

bool ResourceManager::unloadResource(const std::string& name) {
    // Validate name
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return false; // Resource not found
    }
    
    auto& resource = it->second;
    ResourceHandle handle = resource->getHandle();
    
    // Unload resource data
    resource->unload();
    
    // Remove from registries
    m_resourcesByGeneration.erase(handle.generation);
    m_resourcesByIndex.erase(handle.index);
    m_resourcesByName.erase(it);
    
    return true;
}

ResourceBase* ResourceManager::getResource(ResourceHandle handle) {
    // Validate handle
    if (!handle.isValid()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return nullptr; // Resource not found
    }
    
    auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return nullptr; // Stale handle
    }
    
    return resource.get();
}

const ResourceBase* ResourceManager::getResource(ResourceHandle handle) const {
    // Validate handle
    if (!handle.isValid()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return nullptr; // Resource not found
    }
    
    const auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return nullptr; // Stale handle
    }
    
    return resource.get();
}

ResourceBase* ResourceManager::getResource(const std::string& name) {
    // Validate name
    if (name.empty()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return nullptr; // Resource not found
    }
    
    return it->second.get();
}

const ResourceBase* ResourceManager::getResource(const std::string& name) const {
    // Validate name
    if (name.empty()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return nullptr; // Resource not found
    }
    
    return it->second.get();
}

bool ResourceManager::hasResource(ResourceHandle handle) const {
    // Validate handle
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return false; // Resource not found
    }
    
    // Check generation to prevent stale handle use
    return it->second->getHandle().generation == handle.generation;
}

bool ResourceManager::hasResource(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_resourcesByName.find(name) != m_resourcesByName.end();
}

std::vector<ResourceHandle> ResourceManager::getAllResources() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ResourceHandle> handles;
    handles.reserve(m_resourcesByIndex.size());
    
    for (const auto& pair : m_resourcesByIndex) {
        handles.push_back(pair.second->getHandle());
    }
    
    return handles;
}

std::vector<ResourceHandle> ResourceManager::getResourcesByType(ResourceType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ResourceHandle> handles;
    
    for (const auto& pair : m_resourcesByIndex) {
        if (pair.second->getType() == type) {
            handles.push_back(pair.second->getHandle());
        }
    }
    
    return handles;
}

size_t ResourceManager::getResourceCount() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_resourcesByIndex.size();
}

size_t ResourceManager::getTotalMemoryUsage() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t total = 0;
    for (const auto& pair : m_resourcesByIndex) {
        total += pair.second->getMemorySize();
    }
    
    return total;
}

void ResourceManager::clear() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& pair : m_resourcesByIndex) {
        pair.second->unload();
    }
    
    m_resourcesByIndex.clear();
    m_resourcesByName.clear();
    m_resourcesByGeneration.clear();
}

bool ResourceManager::reloadResource(ResourceHandle handle) {
    // Validate handle
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return false; // Resource not found
    }
    
    auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return false; // Stale handle
    }
    
    // Reload resource
    resource->setState(ResourceState::Loading);
    if (!resource->load()) {
        resource->setState(ResourceState::Failed);
        return false;
    }
    
    resource->setState(ResourceState::Loaded);
    return true;
}

bool ResourceManager::reloadResource(const std::string& name) {
    // Validate name
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return false; // Resource not found
    }
    
    auto& resource = it->second;
    
    // Reload resource
    resource->setState(ResourceState::Loading);
    if (!resource->load()) {
        resource->setState(ResourceState::Failed);
        return false;
    }
    
    resource->setState(ResourceState::Loaded);
    return true;
}

ResourceState ResourceManager::getResourceState(ResourceHandle handle) const {
    // Validate handle
    if (!handle.isValid()) {
        return ResourceState::Failed;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return ResourceState::Failed; // Resource not found
    }
    
    const auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return ResourceState::Failed; // Stale handle
    }
    
    return resource->getState();
}

ResourceState ResourceManager::getResourceState(const std::string& name) const {
    // Validate name
    if (name.empty()) {
        return ResourceState::Failed;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return ResourceState::Failed; // Resource not found
    }
    
    return it->second->getState();
}

bool ResourceManager::getResourceInfo(ResourceHandle handle, ResourceInfo& info) const {
    // Validate handle
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByIndex.find(handle.index);
    if (it == m_resourcesByIndex.end()) {
        return false; // Resource not found
    }
    
    const auto& resource = it->second;
    
    // Check generation to prevent stale handle use
    if (resource->getHandle().generation != handle.generation) {
        return false; // Stale handle
    }
    
    info.name = resource->getName();
    info.filepath = resource->getFilePath();
    info.type = resource->getType();
    info.state = resource->getState();
    info.memorySize = resource->getMemorySize();
    info.refCount = resource->getRefCount();
    
    return true;
}

bool ResourceManager::getResourceInfo(const std::string& name, ResourceInfo& info) const {
    // Validate name
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_resourcesByName.find(name);
    if (it == m_resourcesByName.end()) {
        return false; // Resource not found
    }
    
    const auto& resource = it->second;
    
    info.name = resource->getName();
    info.filepath = resource->getFilePath();
    info.type = resource->getType();
    info.state = resource->getState();
    info.memorySize = resource->getMemorySize();
    info.refCount = resource->getRefCount();
    
    return true;
}

} // namespace resources
} // namespace core
} // namespace poko
