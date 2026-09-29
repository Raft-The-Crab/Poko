/**
 * @file getter.cpp
 * @brief Global system allocator getter implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#include <mutex>

namespace poko {
namespace core {
namespace memory {

SystemAllocator& getSystemAllocator() {
    // Use static local instance with mutex for thread-safe initialization
    static std::mutex mutex;
    static SystemAllocator allocator;
    
    std::lock_guard<std::mutex> lock(mutex);
    if (!g_systemAllocator) {
        g_systemAllocator = &allocator;
    }
    
    return *g_systemAllocator;
}

} // namespace memory
} // namespace core
} // namespace poko