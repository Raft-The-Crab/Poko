/**
 * @file constructor.cpp
 * @brief BinaryMemorySerializer constructor implementation
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

BinaryMemorySerializer::BinaryMemorySerializer(size_t initialCapacity)
    : m_mode(SerializeMode::Write)
    , m_buffer()
    , m_position(0)
{
    m_buffer.reserve(initialCapacity);
}

BinaryMemorySerializer::BinaryMemorySerializer(const void* data, size_t size)
    : m_mode(SerializeMode::Read)
    , m_buffer()
    , m_position(0)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    m_buffer.assign(bytes, bytes + size);
}

BinaryMemorySerializer::~BinaryMemorySerializer() = default;

} // namespace serialization
} // namespace core
} // namespace poko
