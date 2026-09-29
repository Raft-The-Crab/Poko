/**
 * @file constructor.cpp
 * @brief Resource base class implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/resources/resource.h"

namespace poko {
namespace core {
namespace resources {

ResourceBase::ResourceBase(const std::string& name, ResourceType type)
    : m_name(name)
    , m_type(type)
    , m_state(ResourceState::Unloaded)
    , m_id(INVALID_RESOURCE_ID)
    , m_handle()
    , m_refCount(0)
    , m_filePath()
{
    // Validate and truncate name length
    if (m_name.empty()) {
        m_name = "UnnamedResource";
    } else if (m_name.length() > MAX_RESOURCE_NAME_LENGTH) {
        m_name = m_name.substr(0, MAX_RESOURCE_NAME_LENGTH);
    }
}

void ResourceBase::setFilePath(const std::string& path) {
    // Validate and truncate filepath length
    if (path.length() > MAX_RESOURCE_FILEPATH_LENGTH) {
        m_filePath = path.substr(0, MAX_RESOURCE_FILEPATH_LENGTH);
    } else {
        m_filePath = path;
    }
}

} // namespace resources
} // namespace core
} // namespace poko
