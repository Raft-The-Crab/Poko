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
#include <limits>

namespace poko {
namespace core {
namespace serialization {

/**
 * @brief Serialize string with length prefix
 * 
 * Format: [uint32_t length][string data]
 * 
 * @param value String to serialize
 * @return True if serialization succeeded
 * 
 * @note Write mode: writes length prefix then string data
 * @note Read mode: reads length prefix then string data
 * @note Maximum string length: 2^32-1 bytes (4GB)
 * @note Thread-safe per serializer instance
 */
bool BinaryMemorySerializer::serialize(std::string& value) {
    if (isWriting()) {
        // Write mode: serialize length then data
        uint32_t size = static_cast<uint32_t>(value.size());
        if (!serialize(size)) return false;
        
        if (size > 0) {
            if (!writeBytes(value.data(), size)) return false;
        }
    } else {
        // Read mode: read length then data
        uint32_t size = 0;
        if (!serialize(size)) return false;
        
        // Sanity check for reasonable string size
        if (size > 1024 * 1024) { // 1MB limit for safety
            return false;
        }
        
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
