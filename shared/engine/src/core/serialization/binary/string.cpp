/**
 * @file string.cpp
 * @brief BinaryMemorySerializer string implementation
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

bool BinaryMemorySerializer::serialize(std::string& value) {
    if (isWriting()) {
        uint32_t size = static_cast<uint32_t>(value.size());
        if (!serialize(size)) return false;
        if (size > 0) {
            if (!writeBytes(value.data(), size)) return false;
        }
    } else {
        uint32_t size = 0;
        if (!serialize(size)) return false;
        value.resize(size);
        if (size > 0) {
            if (!readBytes(&value[0], size)) return false;
        }
    }
    return true;
}

} // namespace serialization
} // namespace core
} // namespace poko
