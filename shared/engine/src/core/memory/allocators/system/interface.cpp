/**
 * @file interface.cpp
 * @brief Implementation of allocator interface and base functionality
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