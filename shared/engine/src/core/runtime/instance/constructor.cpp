/**
 * @file constructor.cpp
 * @brief Instance constructor implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"
#include "core/handles/handle_manager.h"

namespace poko {
namespace core {
namespace runtime {

Instance::Instance(InstanceType type, const std::string& name)
    : m_id()
    , m_type(type)
    , m_name(name)
    , m_state(InstanceState::Created)
    , m_parent()
    , m_children()
    , m_properties()
    , m_tags()
{
    handles::HandleManager& manager = handles::getHandleManager();
    m_id = manager.allocate();
}

Instance::~Instance() {
    handles::HandleManager& manager = handles::getHandleManager();
    (void)manager.free(m_id);
}

} // namespace runtime
} // namespace core
} // namespace poko
