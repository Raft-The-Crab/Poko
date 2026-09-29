/**
 * @file accessors.cpp
 * @brief Instance accessor implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"

namespace poko {
namespace core {
namespace runtime {

handles::Handle Instance::getId() const noexcept {
    return m_id;
}

InstanceType Instance::getType() const noexcept {
    return m_type;
}

const std::string& Instance::getName() const noexcept {
    return m_name;
}

void Instance::setName(const std::string& name) {
    m_name = name;
}

InstanceState Instance::getState() const noexcept {
    return m_state;
}

void Instance::setState(InstanceState state) noexcept {
    m_state = state;
}

handles::Handle Instance::getParent() const noexcept {
    return m_parent;
}

void Instance::setParent(handles::Handle parent) noexcept {
    m_parent = parent;
}

const std::vector<handles::Handle>& Instance::getChildren() const noexcept {
    return m_children;
}

} // namespace runtime
} // namespace core
} // namespace poko
