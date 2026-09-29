/**
 * @file interface.h
 * @brief Core memory allocator interface forward declarations
 */

#ifndef POKO_CORE_MEMORY_ALLOCATORS_INTERFACE_H
#define POKO_CORE_MEMORY_ALLOCATORS_INTERFACE_H

#include <cstddef>
#include <cstdint>

namespace poko {
namespace core {
namespace memory {

// Forward declarations - full definitions are in allocator.h
enum class AllocationFlags : uint32_t;
struct AllocationStats;
class IAllocator;

} // namespace memory
} // namespace core
} // namespace poko

#endif // POKO_CORE_MEMORY_ALLOCATORS_INTERFACE_H