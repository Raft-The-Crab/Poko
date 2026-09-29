/**
 * @file constructor.cpp
 * @brief System allocator constructor and destructor
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

SystemAllocator::SystemAllocator() 
    : m_stats{}, m_trackingEnabled(false) {
}

SystemAllocator::~SystemAllocator() {
    // System allocator doesn't own any resources to clean up
}

} // namespace memory
} // namespace core
} // namespace poko