/**
 * @file world.h
 * @brief World and Scene runtime for Poko Engine
 * @details World contains the runtime hierarchy with support for static geometry, dynamic instances, and streaming
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include "runtime/instance.h"
#include "core/handles/ids.h"

namespace poko {
namespace runtime {

using InstanceHandle = core::InstanceHandle;
using IDGenerator = core::IDGenerator;

/**
 * @enum ObjectLifecycleState
 * @brief Lifecycle states for world objects
 */
enum class ObjectLifecycleState {
    CREATED,     ///< Object has been created but not yet attached
    ATTACHED,    ///< Object is attached to the world
    ACTIVE,      ///< Object is active and being processed
    DORMANT,     ///< Object is inactive but retained
    DESTROYED    ///< Object has been destroyed
};

/**
 * @class WorldObject
 * @brief Wrapper for instances with additional world metadata
 */
class WorldObject {
public:
    /**
     * @brief Construct a world object
     * @param instance The instance to wrap
     * @param type The instance type
     */
    WorldObject(Instance* instance, InstanceType type);

    /**
     * @brief Virtual destructor
     */
    virtual ~WorldObject();

    /**
     * @brief Get the instance
     * @return Pointer to the instance
     */
    Instance* instance() { return instance_; }

    /**
     * @brief Get the instance (const)
     * @return Const pointer to the instance
     */
    const Instance* instance() const { return instance_; }

    /**
     * @brief Get the instance type
     * @return The instance type
     */
    InstanceType type() const { return type_; }

    /**
     * @brief Get the lifecycle state
     * @return Current lifecycle state
     */
    ObjectLifecycleState lifecycle_state() const { return lifecycle_state_; }

    /**
     * @brief Set the lifecycle state
     * @param state The new state
     */
    void set_lifecycle_state(ObjectLifecycleState state) { lifecycle_state_ = state; }

    /**
     * @brief Check if the object is active
     * @return true if active
     */
    bool is_active() const { return lifecycle_state_ == ObjectLifecycleState::ACTIVE; }

    /**
     * @brief Check if the object is destroyed
     * @return true if destroyed
     */
    bool is_destroyed() const { return lifecycle_state_ == ObjectLifecycleState::DESTROYED; }

private:
    Instance* instance_;
    InstanceType type_;
    ObjectLifecycleState lifecycle_state_;
};

/**
 * @class World
 * @brief Contains the runtime hierarchy for a game world
 * @details Manages static geometry, dynamic instances, and supports streaming
 */
class World {
public:
    /**
     * @brief Construct a world
     * @param name The world name
     */
    explicit World(const std::string& name);

    /**
     * @brief Destructor
     */
    ~World();

    /**
     * @brief Get the world name
     * @return The world name
     */
    const std::string& name() const { return name_; }

    /**
     * @brief Create a new instance in the world
     * @param name The instance name
     * @param type The instance type
     * @return Handle to the created instance
     */
    InstanceHandle create_instance(const std::string& name, InstanceType type);

    /**
     * @brief Destroy an instance
     * @param handle Handle to the instance to destroy
     * @return true if successful
     */
    bool destroy_instance(InstanceHandle handle);

    /**
     * @brief Get an instance by handle
     * @param handle Handle to the instance
     * @return Pointer to the instance, or nullptr if not found
     */
    Instance* get_instance(InstanceHandle handle);

    /**
     * @brief Get an instance by handle (const)
     * @param handle Handle to the instance
     * @return Const pointer to the instance, or nullptr if not found
     */
    const Instance* get_instance(InstanceHandle handle) const;

    /**
     * @brief Get a world object by handle
     * @param handle Handle to the instance
     * @return Pointer to the world object, or nullptr if not found
     */
    WorldObject* get_world_object(InstanceHandle handle);

    /**
     * @brief Get all instances of a specific type
     * @param type The instance type to filter by
     * @return Vector of handles to instances of that type
     */
    std::vector<InstanceHandle> get_instances_by_type(InstanceType type) const;

    /**
     * @brief Update the world (called each frame)
     * @param delta_time Time since last update in seconds
     */
    void update(float delta_time);

    /**
     * @brief Get the number of active instances
     * @return Number of active instances
     */
    size_t active_instance_count() const;

    /**
     * @brief Get the total number of instances
     * @return Total number of instances (including inactive)
     */
    size_t total_instance_count() const;

private:
    std::string name_;
    std::map<uint64_t, std::unique_ptr<WorldObject>> objects_;
    IDGenerator id_generator_;
};

/**
 * @class Scene
 * @brief Represents a serializable scene definition
 * @details Scenes retain stable object references for serialization
 */
class Scene {
public:
    /**
     * @brief Construct a scene
     * @param name The scene name
     */
    explicit Scene(const std::string& name);

    /**
     * @brief Destructor
     */
    ~Scene();

    /**
     * @brief Get the scene name
     * @return The scene name
     */
    const std::string& name() const { return name_; }

    /**
     * @brief Load a scene from a world
     * @param world The world to load from
     * @return true if successful
     */
    bool load_from_world(const World& world);

    /**
     * @brief Save a scene to a world
     * @param world The world to save to
     * @return true if successful
     */
    bool save_to_world(World& world) const;

    /**
     * @brief Serialize the scene to binary data
     * @param buffer Output buffer for serialized data
     * @return true if successful
     */
    bool serialize(std::vector<uint8_t>& buffer) const;

    /**
     * @brief Deserialize a scene from binary data
     * @param buffer Input buffer with serialized data
     * @return true if successful
     */
    bool deserialize(const std::vector<uint8_t>& buffer);

private:
    std::string name_;
    std::vector<uint64_t> instance_ids_;
};

} // namespace runtime
} // namespace poko
