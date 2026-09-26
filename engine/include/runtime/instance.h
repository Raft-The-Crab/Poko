/**
 * @file instance.h
 * @brief Instance base class for Poko Engine
 * @details Base class for all engine instances (objects in the scene)
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include "core/handles/ids.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace poko {
namespace runtime {

// Forward declarations
class Instance;
class Component;

/**
 * @enum InstanceType
 * @brief Type categories for instances
 */
enum class InstanceType {
    Base,
    Model,
    Part,
    Mesh,
    Light,
    Camera,
    Script,
    UIElement,
    Custom
};

/**
 * @class Instance
 * @brief Base class for all engine instances
 * @details Provides common functionality: ID, name, parent/child hierarchy, attributes, tags, components
 */
class Instance {
public:
    /**
     * @brief Construct an instance
     * @param name Instance name
     * @param type Instance type
     */
    explicit Instance(const std::string& name, InstanceType type = InstanceType::Base);

    /**
     * @brief Virtual destructor
     */
    virtual ~Instance();

    // Identity
    core::InstanceID id() const { return id_; }
    const std::string& name() const { return name_; }
    void set_name(const std::string& name) { name_ = name; }
    InstanceType type() const { return type_; }

    // Parent/child hierarchy
    Instance* parent() const { return parent_; }
    void set_parent(Instance* parent);
    const std::vector<Instance*>& children() const { return children_; }
    void add_child(Instance* child);
    void remove_child(Instance* child);

    // Attributes (key-value properties)
    void set_attribute(const std::string& key, const std::string& value);
    std::string get_attribute(const std::string& key) const;
    bool has_attribute(const std::string& key) const;
    void remove_attribute(const std::string& key);

    // Tags
    void add_tag(const std::string& tag);
    void remove_tag(const std::string& tag);
    bool has_tag(const std::string& tag) const;
    const std::vector<std::string>& tags() const { return tags_; }

    // Components
    template<typename T>
    T* add_component();

    template<typename T>
    T* get_component() const;

    template<typename T>
    void remove_component();

    // Lifecycle
    virtual void awake() {}
    virtual void start() {}
    virtual void update(float delta_time) { (void)delta_time; }
    virtual void fixed_update(float fixed_delta_time) { (void)fixed_delta_time; }
    virtual void late_update(float delta_time) { (void)delta_time; }
    virtual void on_destroy() {}

protected:
    core::InstanceID id_;
    std::string name_;
    InstanceType type_;

    // Hierarchy
    Instance* parent_;
    std::vector<Instance*> children_;

    // Attributes
    std::unordered_map<std::string, std::string> attributes_;

    // Tags
    std::vector<std::string> tags_;

    // Components
    std::vector<std::unique_ptr<Component>> components_;
};

// Template implementations (must be in header)
template<typename T>
T* Instance::add_component() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    auto component = std::make_unique<T>(this);
    T* raw_ptr = component.get();
    components_.push_back(std::move(component));
    return raw_ptr;
}

template<typename T>
T* Instance::get_component() const {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    for (const auto& component : components_) {
        if (auto* casted = dynamic_cast<T*>(component.get())) {
            return casted;
        }
    }
    return nullptr;
}

template<typename T>
void Instance::remove_component() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    auto it = std::remove_if(components_.begin(), components_.end(),
        [](const std::unique_ptr<Component>& comp) {
            return dynamic_cast<T*>(comp.get()) != nullptr;
        });
    for (auto i = it; i != components_.end(); ++i) {
        (*i)->on_destroy();
    }
    components_.erase(it, components_.end());
}

/**
 * @class Component
 * @brief Base class for instance components
 * @details Components add modular functionality to instances
 */
class Component {
public:
    /**
     * @brief Construct a component
     * @param owner Owning instance
     */
    explicit Component(Instance* owner);

    /**
     * @brief Virtual destructor
     */
    virtual ~Component() = default;

    /**
     * @brief Get the owning instance
     * @return Owning instance
     */
    Instance* owner() const { return owner_; }

    // Lifecycle
    virtual void awake() {}
    virtual void start() {}
    virtual void update(float delta_time) { (void)delta_time; }
    virtual void fixed_update(float fixed_delta_time) { (void)fixed_delta_time; }
    virtual void late_update(float delta_time) { (void)delta_time; }
    virtual void on_destroy() {}

protected:
    Instance* owner_;
};

} // namespace runtime
} // namespace poko
