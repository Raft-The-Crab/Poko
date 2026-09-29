/**
 * @file get.cpp
 * @brief Property getter implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"

namespace poko {
namespace core {
namespace properties {

const PropertyValue& Property::getValue() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_value;
}

const PropertyMetadata& Property::getMetadata() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metadata;
}

const std::string& Property::getName() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metadata.name;
}

PropertyType Property::getType() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metadata.type;
}

bool Property::isReadOnly() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return hasFlag(m_metadata.flags, PropertyFlags::ReadOnly);
}

bool Property::isTransient() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return hasFlag(m_metadata.flags, PropertyFlags::Transient);
}

} // namespace properties
} // namespace core
} // namespace poko
