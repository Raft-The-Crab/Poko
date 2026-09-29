/**
 * @file info.cpp
 * @brief System allocator info implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"

namespace poko {
namespace core {
namespace memory {

const char* SystemAllocator::getName() const noexcept {
    return "SystemAllocator";
}

bool SystemAllocator::supportsIndividualDeallocation() const noexcept {
    return true;
}

} // namespace memory
} // namespace core
} // namespace poko