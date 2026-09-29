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
    // Validate tag length
    if (tag.length() > MAX_TAG_LENGTH) {
        return;
    }
    
    // Check tag count limit
    if (m_tags.size() >= MAX_TAGS_PER_INSTANCE) {
        return;
    }
    
    // Check if tag already exists
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it == m_tags.end()) {
        m_tags.push_back(tag);
    }
}

void Instance::addTag(std::string&& tag) noexcept {
    // Validate tag length
    if (tag.length() > MAX_TAG_LENGTH) {
        return;
    }
    
    // Check tag count limit
    if (m_tags.size() >= MAX_TAGS_PER_INSTANCE) {
        return;
    }
    
    // Check if tag already exists
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it == m_tags.end()) {
        m_tags.push_back(std::move(tag));
    }
}

void Instance::removeTag(const std::string& tag) noexcept {
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it != m_tags.end()) {
        m_tags.erase(it);
    }
}

bool Instance::hasTag(const std::string& tag) const noexcept {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

const std::vector<std::string>& Instance::getTags() const noexcept {
    return m_tags;
}

} // namespace runtime
} // namespace core
} // namespace poko
