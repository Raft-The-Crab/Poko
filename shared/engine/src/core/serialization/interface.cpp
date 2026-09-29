/**
 * @file interface.cpp
 * @brief Global serializer access implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/serialization/serializer.h"
#include <memory>

namespace poko {
namespace core {
namespace serialization {

std::unique_ptr<BinaryMemorySerializer> createBinaryWriter(size_t initialCapacity) {
    return std::make_unique<BinaryMemorySerializer>(initialCapacity);
}

std::unique_ptr<BinaryMemorySerializer> createBinaryReader(const void* data, size_t size) {
    return std::make_unique<BinaryMemorySerializer>(data, size);
}

} // namespace serialization
} // namespace core
} // namespace poko
