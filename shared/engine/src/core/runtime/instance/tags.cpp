/**
 * @file tags.cpp
 * @brief Instance tags implementation
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

void Instance::addTag(const std::string& tag) {
    // Check if tag already exists
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it == m_tags.end()) {
        m_tags.push_back(tag);
    }
}

void Instance::removeTag(const std::string& tag) {
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it != m_tags.end()) {
        m_tags.erase(it);
    }
}

bool Instance::hasTag(const std::string& tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

const std::vector<std::string>& Instance::getTags() const noexcept {
    return m_tags;
}

} // namespace runtime
} // namespace core
} // namespace poko
