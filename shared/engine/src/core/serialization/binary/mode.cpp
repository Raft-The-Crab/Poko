/**
 * @file mode.cpp
 * @brief BinaryMemorySerializer mode implementation
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

SerializeMode BinaryMemorySerializer::getMode() const noexcept {
    return m_mode;
}

bool BinaryMemorySerializer::isReading() const noexcept {
    return m_mode == SerializeMode::Read;
}

bool BinaryMemorySerializer::isWriting() const noexcept {
    return m_mode == SerializeMode::Write;
}

} // namespace serialization
} // namespace core
} // namespace poko
