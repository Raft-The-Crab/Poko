/**
 * @file children.cpp
 * @brief Instance children management implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"
#include <algorithm>

namespace poko {
namespace core {
namespace runtime {

void Instance::addChild(handles::Handle child) noexcept {
    // Check if child already exists
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it == m_children.end()) {
        m_children.push_back(child);
    }
}

void Instance::removeChild(handles::Handle child) noexcept {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
    }
}

} // namespace runtime
} // namespace core
} // namespace poko
