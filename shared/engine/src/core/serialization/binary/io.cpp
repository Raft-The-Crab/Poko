/**
 * @file io.cpp
 * @brief BinaryMemorySerializer I/O implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/serialization/serializer.h"
#include <cstring>

namespace poko {
namespace core {
namespace serialization {

bool BinaryMemorySerializer::readBytes(void* data, size_t size) {
    if (m_position + size > m_buffer.size()) {
        return false; // Out of bounds
    }
    
    std::memcpy(data, m_buffer.data() + m_position, size);
    m_position += size;
    return true;
}

bool BinaryMemorySerializer::writeBytes(const void* data, size_t size) {
    if (m_position + size > m_buffer.size()) {
        // Need to expand buffer
        m_buffer.resize(m_position + size);
    }
    
    std::memcpy(m_buffer.data() + m_position, data, size);
    m_position += size;
    return true;
}

} // namespace serialization
} // namespace core
} // namespace poko
