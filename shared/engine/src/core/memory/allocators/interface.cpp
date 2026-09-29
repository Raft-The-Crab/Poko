/**
 * @file interface.cpp
 * @brief Implementation of allocator interface global variables
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

// Global system allocator instance
SystemAllocator* g_systemAllocator = nullptr;

} // namespace memory
} // namespace core
} // namespace poko