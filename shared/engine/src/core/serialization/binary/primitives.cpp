/**
 * @file primitives.cpp
 * @brief BinaryMemorySerializer primitive type implementation
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

bool BinaryMemorySerializer::serialize(bool& value) {
    if (isWriting()) {
        uint8_t byte = value ? 1 : 0;
        return writeBytes(&byte, sizeof(byte));
    } else {
        uint8_t byte = 0;
        if (!readBytes(&byte, sizeof(byte))) {
            return false;
        }
        value = (byte != 0);
        return true;
    }
}

bool BinaryMemorySerializer::serialize(int8_t& value) {
    return serialize(reinterpret_cast<uint8_t&>(value));
}

bool BinaryMemorySerializer::serialize(int16_t& value) {
    return serialize(reinterpret_cast<uint16_t&>(value));
}

bool BinaryMemorySerializer::serialize(int32_t& value) {
    return serialize(reinterpret_cast<uint32_t&>(value));
}

bool BinaryMemorySerializer::serialize(int64_t& value) {
    return serialize(reinterpret_cast<uint64_t&>(value));
}

bool BinaryMemorySerializer::serialize(uint8_t& value) {
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

bool BinaryMemorySerializer::serialize(uint16_t& value) {
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

bool BinaryMemorySerializer::serialize(uint32_t& value) {
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

bool BinaryMemorySerializer::serialize(uint64_t& value) {
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

bool BinaryMemorySerializer::serialize(float& value) {
    static_assert(sizeof(float) == 4, "Float must be 4 bytes");
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

bool BinaryMemorySerializer::serialize(double& value) {
    static_assert(sizeof(double) == 8, "Double must be 8 bytes");
    if (isWriting()) {
        return writeBytes(&value, sizeof(value));
    } else {
        return readBytes(&value, sizeof(value));
    }
}

} // namespace serialization
} // namespace core
} // namespace poko
