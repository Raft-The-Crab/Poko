/**
 * @file world.cpp
 * @brief Implementation of World and Scene runtime
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "runtime/world.h"
#include "core/logging/logger.h"
#include <algorithm>

namespace poko {
namespace runtime {

// WorldObject implementation
WorldObject::WorldObject(Instance* instance, InstanceType type)
    : instance_(instance)
    , type_(type)
    , lifecycle_state_(ObjectLifecycleState::CREATED) {
}

WorldObject::~WorldObject() {
    // Instance is managed externally
}

// World implementation
World::World(const std::string& name)
    : name_(name)
    , id_generator_(1) {
    POKO_LOG_INFO("Created world: " + name_);
}

World::~World() {
    POKO_LOG_INFO("Destroying world: " + name_);
    // Delete all instances
    for (auto& pair : objects_) {
        delete pair.second->instance();
    }
    objects_.clear();
}

InstanceHandle World::create_instance(const std::string& name, InstanceType type) {
    Instance* instance = new Instance(name, type);
    
    auto world_object = std::make_unique<WorldObject>(instance, type);
    world_object->set_lifecycle_state(ObjectLifecycleState::ATTACHED);
    
    uint64_t id = id_generator_.generate();
    InstanceHandle handle(id, 0);
    objects_[id] = std::move(world_object);
    
    POKO_LOG_INFO("Created instance '" + name + "' in world '" + name_ + "'");
    return handle;
}

bool World::destroy_instance(InstanceHandle handle) {
    auto it = objects_.find(handle.id());
    if (it == objects_.end()) {
        POKO_LOG_WARN("Attempted to destroy non-existent instance");
        return false;
    }
    
    it->second->set_lifecycle_state(ObjectLifecycleState::DESTROYED);
    delete it->second->instance();
    objects_.erase(it);
    
    POKO_LOG_INFO("Destroyed instance in world '" + name_ + "'");
    return true;
}

Instance* World::get_instance(InstanceHandle handle) {
    auto it = objects_.find(handle.id());
    if (it != objects_.end() && !it->second->is_destroyed()) {
        return it->second->instance();
    }
    return nullptr;
}

const Instance* World::get_instance(InstanceHandle handle) const {
    auto it = objects_.find(handle.id());
    if (it != objects_.end() && !it->second->is_destroyed()) {
        return it->second->instance();
    }
    return nullptr;
}

WorldObject* World::get_world_object(InstanceHandle handle) {
    auto it = objects_.find(handle.id());
    if (it != objects_.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<InstanceHandle> World::get_instances_by_type(InstanceType type) const {
    std::vector<InstanceHandle> result;
    for (const auto& pair : objects_) {
        if (pair.second->type() == type && !pair.second->is_destroyed()) {
            result.push_back(InstanceHandle(pair.first, 0));
        }
    }
    return result;
}

void World::update(float delta_time) {
    (void)delta_time;
    // Update all active instances
    for (auto& pair : objects_) {
        if (pair.second->is_active()) {
            // Instance update hooks will be called here
        }
    }
}

size_t World::active_instance_count() const {
    size_t count = 0;
    for (const auto& pair : objects_) {
        if (pair.second->is_active()) {
            count++;
        }
    }
    return count;
}

size_t World::total_instance_count() const {
    return objects_.size();
}

// Scene implementation
Scene::Scene(const std::string& name)
    : name_(name) {
    POKO_LOG_INFO("Created scene: " + name_);
}

Scene::~Scene() {
    POKO_LOG_INFO("Destroying scene: " + name_);
}

bool Scene::load_from_world(const World& world) {
    (void)world;
    POKO_LOG_WARN("Scene::load_from_world not yet implemented");
    return false;
}

bool Scene::save_to_world(World& world) const {
    (void)world;
    POKO_LOG_WARN("Scene::save_to_world not yet implemented");
    return false;
}

bool Scene::serialize(std::vector<uint8_t>& buffer) const {
    (void)buffer;
    POKO_LOG_WARN("Scene::serialize not yet implemented");
    return false;
}

bool Scene::deserialize(const std::vector<uint8_t>& buffer) {
    (void)buffer;
    POKO_LOG_WARN("Scene::deserialize not yet implemented");
    return false;
}

} // namespace runtime
} // namespace poko
