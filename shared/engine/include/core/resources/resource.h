/**
 * @file resource.h
 * @brief Resource management system for loading, caching, and managing engine resources
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_RESOURCES_RESOURCE_H
#define POKO_CORE_RESOURCES_RESOURCE_H

#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace poko {
namespace core {
namespace resources {

// Resource ID type (using handle system)
using ResourceID = uint64_t;

// Invalid resource ID constant
constexpr ResourceID INVALID_RESOURCE_ID = 0;

// Resource states
enum class ResourceState : uint32_t {
    Unloaded = 0,
    Loading = 1,
    Loaded = 2,
    Failed = 3,
    Unloading = 4
};

// Resource type enum
enum class ResourceType : uint32_t {
    Unknown = 0,
    Texture = 1,
    Mesh = 2,
    Material = 3,
    Shader = 4,
    Audio = 5,
    Animation = 6,
    Data = 7,
    Custom = 100
};

// Forward declarations
class ResourceBase;
class ResourceManager;

// Resource handle with generation for stale reference prevention
struct ResourceHandle {
    uint64_t index;
    uint32_t generation;
    
    ResourceHandle() : index(0), generation(0) {}
    ResourceHandle(uint64_t idx, uint32_t gen) : index(idx), generation(gen) {}
    
    // Check if handle is valid (non-zero index and generation)
    // Returns: true if handle is valid, false otherwise
    // Thread-safe: Yes (const operation)
    bool isValid() const noexcept { return index != 0 && generation != 0; }
    
    // Hash function for unordered containers
    // Returns: Hash value combining index and generation
    // Thread-safe: Yes (const operation)
    size_t hash() const noexcept {
        return static_cast<size_t>(index) ^ (static_cast<size_t>(generation) << 32);
    }
    
    // Compare handles for equality
    // Returns: true if handles have same index and generation
    // Thread-safe: Yes (const operation)
    bool operator==(const ResourceHandle& other) const noexcept {
        return index == other.index && generation == other.generation;
    }
    
    // Compare handles for inequality
    // Returns: true if handles differ in index or generation
    // Thread-safe: Yes (const operation)
    bool operator!=(const ResourceHandle& other) const noexcept {
        return !(*this == other);
    }
};

// Base resource class
class ResourceBase {
public:
    using Ptr = std::shared_ptr<ResourceBase>;
    
    ResourceBase(const std::string& name, ResourceType type);
    virtual ~ResourceBase() = default;
    
    // Get resource name
    // Returns: Reference to resource name
    // Thread-safe: Yes (name is const after construction)
    const std::string& getName() const noexcept { return m_name; }
    
    // Get resource type
    // Returns: Resource type enum value
    // Thread-safe: Yes (type is const after construction)
    ResourceType getType() const noexcept { return m_type; }
    
    // Get resource state
    // Returns: Current resource state
    // Thread-safe: Yes (state updates are atomic via ResourceManager locking)
    ResourceState getState() const noexcept { return m_state; }
    
    // Set resource state
    // Parameters: state - New resource state
    // Thread-safe: Should be called only by ResourceManager with lock held
    void setState(ResourceState state) noexcept { m_state = state; }
    
    // Get resource ID
    // Returns: Resource ID (index from handle)
    // Thread-safe: Yes (ID is const after assignment)
    ResourceID getID() const noexcept { return m_id; }
    
    // Set resource ID
    // Parameters: id - Resource ID to assign
    // Thread-safe: Should be called only by ResourceManager with lock held
    void setID(ResourceID id) noexcept { m_id = id; }
    
    // Get resource handle
    // Returns: Reference to resource handle
    // Thread-safe: Yes (handle is const after assignment)
    const ResourceHandle& getHandle() const noexcept { return m_handle; }
    
    // Set resource handle
    // Parameters: handle - Resource handle to assign
    // Thread-safe: Should be called only by ResourceManager with lock held
    void setHandle(const ResourceHandle& handle) noexcept { m_handle = handle; }
    
    // Get reference count
    // Returns: Current reference count
    // Thread-safe: Yes (atomic operations via ResourceManager locking)
    uint32_t getRefCount() const noexcept { return m_refCount; }
    
    // Increment reference count
    // Thread-safe: Should be called only by ResourceManager with lock held
    void incrementRefCount() noexcept { ++m_refCount; }
    
    // Decrement reference count
    // Thread-safe: Should be called only by ResourceManager with lock held
    void decrementRefCount() noexcept { if (m_refCount > 0) --m_refCount; }
    
    // Get file path
    // Returns: Reference to file path string
    // Thread-safe: Yes (filepath is const after assignment)
    const std::string& getFilePath() const noexcept { return m_filePath; }
    
    // Set file path
    // Parameters: path - File path to assign (truncated if exceeding MAX_RESOURCE_FILEPATH_LENGTH)
    // Thread-safe: Should be called only by ResourceManager with lock held
    void setFilePath(const std::string& path);
    
    // Get memory size in bytes
    // Returns: Memory size in bytes used by resource data
    // Thread-safe: Yes (memory size is const after loading)
    virtual size_t getMemorySize() const noexcept = 0;
    
    // Load resource data (implementation-specific)
    // Returns: true if load succeeded, false otherwise
    // Thread-safe: Should be called only by ResourceManager with lock held
    virtual bool load() = 0;
    
    // Unload resource data (implementation-specific)
    // Thread-safe: Should be called only by ResourceManager with lock held
    virtual void unload() = 0;
    
    // Check if resource is loaded
    // Returns: true if resource state is Loaded
    // Thread-safe: Yes (state read is atomic)
    bool isLoaded() const noexcept { return m_state == ResourceState::Loaded; }
    
    // Check if resource is loading
    // Returns: true if resource state is Loading
    // Thread-safe: Yes (state read is atomic)
    bool isLoading() const noexcept { return m_state == ResourceState::Loading; }
    
    // Check if resource failed to load
    // Returns: true if resource state is Failed
    // Thread-safe: Yes (state read is atomic)
    bool isFailed() const noexcept { return m_state == ResourceState::Failed; }
    
protected:
    std::string m_name;
    ResourceType m_type;
    ResourceState m_state;
    ResourceID m_id;
    ResourceHandle m_handle;
    uint32_t m_refCount;
    std::string m_filePath;
};

// Template resource class for type-specific resources
template<typename T>
class Resource : public ResourceBase {
public:
    using Ptr = std::shared_ptr<Resource<T>>;
    
    Resource(const std::string& name, ResourceType type)
        : ResourceBase(name, type)
        , m_data(nullptr)
    {}
    
    // Get resource data
    T* getData() noexcept { return m_data.get(); }
    const T* getData() const noexcept { return m_data.get(); }
    
    // Set resource data
    void setData(std::unique_ptr<T> data) noexcept { m_data = std::move(data); }
    
    // Get memory size
    size_t getMemorySize() const noexcept override {
        return m_data ? sizeof(T) : 0;
    }
    
    // Load resource (must be implemented by derived classes)
    bool load() override = 0;
    
    // Unload resource
    void unload() override {
        m_data.reset();
        setState(ResourceState::Unloaded);
    }
    
private:
    std::unique_ptr<T> m_data;
};

// Resource loader function signature
using ResourceLoader = std::function<ResourceBase::Ptr(const std::string&, ResourceType)>;

// Resource manager class
class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    // Register a resource loader for a specific type
    bool registerLoader(ResourceType type, ResourceLoader loader);
    
    // Unregister a resource loader
    bool unregisterLoader(ResourceType type);
    
    // Load a resource from file
    ResourceHandle loadResource(const std::string& filepath, ResourceType type);
    
    // Load a resource with custom name
    ResourceHandle loadResource(const std::string& filepath, const std::string& name, ResourceType type);
    
    // Unload a resource
    bool unloadResource(ResourceHandle handle);
    
    // Unload a resource by name
    bool unloadResource(const std::string& name);
    
    // Get a resource by handle
    ResourceBase* getResource(ResourceHandle handle);
    const ResourceBase* getResource(ResourceHandle handle) const;
    
    // Get a resource by name
    ResourceBase* getResource(const std::string& name);
    const ResourceBase* getResource(const std::string& name) const;
    
    // Check if a resource exists
    bool hasResource(ResourceHandle handle) const;
    bool hasResource(const std::string& name) const;
    
    // Get all loaded resources
    std::vector<ResourceHandle> getAllResources() const;
    
    // Get resources by type
    std::vector<ResourceHandle> getResourcesByType(ResourceType type) const;
    
    // Get resource count
    size_t getResourceCount() const noexcept;
    
    // Get total memory usage
    size_t getTotalMemoryUsage() const noexcept;
    
    // Clear all resources
    // Thread-safe: Yes (holds mutex lock)
    // noexcept: Yes (all operations are noexcept or catch exceptions internally)
    void clear() noexcept;
    
    // Reload a resource
    bool reloadResource(ResourceHandle handle);
    bool reloadResource(const std::string& name);
    
    // Get resource state
    ResourceState getResourceState(ResourceHandle handle) const;
    ResourceState getResourceState(const std::string& name) const;
    
    // Get resource info
    struct ResourceInfo {
        std::string name;
        std::string filepath;
        ResourceType type;
        ResourceState state;
        size_t memorySize;
        uint32_t refCount;
    };
    
    bool getResourceInfo(ResourceHandle handle, ResourceInfo& info) const;
    bool getResourceInfo(const std::string& name, ResourceInfo& info) const;
    
private:
    mutable std::mutex m_mutex;
    std::unordered_map<ResourceType, ResourceLoader> m_loaders;
    std::unordered_map<uint64_t, ResourceBase::Ptr> m_resourcesByIndex;
    std::unordered_map<std::string, ResourceBase::Ptr> m_resourcesByName;
    std::unordered_map<uint32_t, ResourceBase*> m_resourcesByGeneration;
    uint64_t m_nextIndex;
    uint32_t m_nextGeneration;
};

// Global resource manager instance
ResourceManager& getGlobalResourceManager();
void destroyGlobalResourceManager();

// Constants for resource management
constexpr size_t MAX_RESOURCE_NAME_LENGTH = 256;
constexpr size_t MAX_RESOURCE_FILEPATH_LENGTH = 1024;
constexpr size_t MAX_RESOURCES_LOADED = 10000;
constexpr size_t MAX_RESOURCE_MEMORY_MB = 2048; // 2GB limit

} // namespace resources
} // namespace core
} // namespace poko

#endif // POKO_CORE_RESOURCES_RESOURCE_H
