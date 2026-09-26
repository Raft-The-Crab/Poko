/**
 * @file instance.cpp
 * @brief Instance implementation for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "runtime/instance.h"
#include "core/handles/ids.h"
#include <algorithm>

namespace poko {
namespace runtime {

static core::IDGenerator instance_id_generator(1);

Instance::Instance(const std::string& name, InstanceType type)
    : id_(instance_id_generator.generate())
    , name_(name)
    , type_(type)
    , parent_(nullptr) {
}

Instance::~Instance() {
    // Remove from parent if we have one
    if (parent_) {
        parent_->remove_child(this);
    }

    // Destroy all children
    for (Instance* child : children_) {
        child->parent_ = nullptr;
        delete child;
    }
    children_.clear();

    // Destroy all components
    for (auto& component : components_) {
        component->on_destroy();
    }
    components_.clear();
}

void Instance::set_parent(Instance* parent) {
    if (parent_ == parent) return;

    // Remove from current parent
    if (parent_) {
        parent_->remove_child(this);
    }

    // Set new parent
    parent_ = parent;

    // Add to new parent's children
    if (parent_) {
        parent_->add_child(this);
    }
}

void Instance::add_child(Instance* child) {
    if (child && std::find(children_.begin(), children_.end(), child) == children_.end()) {
        children_.push_back(child);
        child->parent_ = this;
    }
}

void Instance::remove_child(Instance* child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->parent_ = nullptr;
        children_.erase(it);
    }
}

void Instance::set_attribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string Instance::get_attribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return "";
}

bool Instance::has_attribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void Instance::remove_attribute(const std::string& key) {
    attributes_.erase(key);
}

void Instance::add_tag(const std::string& tag) {
    if (std::find(tags_.begin(), tags_.end(), tag) == tags_.end()) {
        tags_.push_back(tag);
    }
}

void Instance::remove_tag(const std::string& tag) {
    auto it = std::find(tags_.begin(), tags_.end(), tag);
    if (it != tags_.end()) {
        tags_.erase(it);
    }
}

bool Instance::has_tag(const std::string& tag) const {
    return std::find(tags_.begin(), tags_.end(), tag) != tags_.end();
}

Component::Component(Instance* owner)
    : owner_(owner) {
}

} // namespace runtime
} // namespace poko
