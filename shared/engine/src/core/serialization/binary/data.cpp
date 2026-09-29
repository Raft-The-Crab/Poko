/**
 * @file data.cpp
 * @brief BinaryMemorySerializer data access implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/serialization/serializer.h"

namespace poko {
namespace core {
namespace serialization {

const uint8_t* BinaryMemorySerializer::getData() const noexcept {
    return m_buffer.data();
}

size_t BinaryMemorySerializer::getSize() const noexcept {
    return m_buffer.size();
}

void BinaryMemorySerializer::reset() noexcept {
    m_position = 0;
    if (isWriting()) {
        m_buffer.clear();
    }
}

} // namespace serialization
} // namespace core
} // namespace poko
