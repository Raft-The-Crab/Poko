/**
 * @file constructor.cpp
 * @brief Property constructor implementation
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

Property::Property(const PropertyMetadata& metadata)
    : m_metadata(metadata)
    , m_value(metadata.defaultValue)
    , m_callback(nullptr)
{
}

Property::Property(const PropertyMetadata& metadata, const PropertyValue& value)
    : m_metadata(metadata)
    , m_value(value)
    , m_callback(nullptr)
{
}

Property::Property(const Property& other)
    : m_metadata(other.m_metadata)
    , m_value(other.m_value)
    , m_callback(other.m_callback)
{
}

Property::Property(Property&& other) noexcept
    : m_metadata(std::move(other.m_metadata))
    , m_value(std::move(other.m_value))
    , m_callback(std::move(other.m_callback))
{
}

Property& Property::operator=(const Property& other) {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::lock_guard<std::mutex> otherLock(other.m_mutex);
        
        m_metadata = other.m_metadata;
        m_value = other.m_value;
        m_callback = other.m_callback;
    }
    return *this;
}

Property& Property::operator=(Property&& other) noexcept {
    if (this != &other) {
        std::lock(m_mutex, other.m_mutex);
        std::lock_guard<std::mutex> lock(m_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> otherLock(other.m_mutex, std::adopt_lock);
        
        m_metadata = std::move(other.m_metadata);
        m_value = std::move(other.m_value);
        m_callback = std::move(other.m_callback);
    }
    return *this;
}

} // namespace properties
} // namespace core
} // namespace poko
