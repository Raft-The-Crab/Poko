/**
 * @file memory.cpp
 * @brief Implementation of memory utilities and allocators
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/memory/memory.h"
#include "core/logging/logger.h"
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#define POKO_ALIGNED_ALLOC(size, alignment) _aligned_malloc(size, alignment)
#define POKO_ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
#define POKO_ALIGNED_ALLOC(size, alignment) std::aligned_alloc(alignment, size)
#define POKO_ALIGNED_FREE(ptr) std::free(ptr)
#endif

namespace poko {
namespace core {
namespace memory {

namespace {
    std::mutex& get_tracker_mutex() {
        static std::mutex mutex;
        return mutex;
    }
}

// MemoryTracker implementation
MemoryTracker& MemoryTracker::instance() {
    static MemoryTracker instance;
    return instance;
}

MemoryTracker::MemoryTracker() : enabled_(true) {
    std::memset(stats_, 0, sizeof(stats_));
}

void MemoryTracker::record_allocation(size_t size, MemoryCategory category) {
    if (!enabled_) return;

    std::lock_guard<std::mutex> lock(get_tracker_mutex());
    size_t index = static_cast<size_t>(category);
    stats_[index].total_allocated += size;
    stats_[index].current_usage += size;
    stats_[index].allocation_count++;

    if (stats_[index].current_usage > stats_[index].peak_usage) {
        stats_[index].peak_usage = stats_[index].current_usage;
    }
}

void MemoryTracker::record_deallocation(size_t size, MemoryCategory category) {
    if (!enabled_) return;

    std::lock_guard<std::mutex> lock(get_tracker_mutex());
    size_t index = static_cast<size_t>(category);
    stats_[index].total_freed += size;
    stats_[index].current_usage -= size;
    stats_[index].deallocation_count++;
}

MemoryStats MemoryTracker::get_stats(MemoryCategory category) const {
    size_t index = static_cast<size_t>(category);
    return stats_[index];
}

MemoryStats MemoryTracker::get_total_stats() const {
    MemoryStats total = {};
    for (size_t i = 0; i < static_cast<size_t>(MemoryCategory::COUNT); ++i) {
        total.total_allocated += stats_[i].total_allocated;
        total.total_freed += stats_[i].total_freed;
        total.current_usage += stats_[i].current_usage;
        total.peak_usage += stats_[i].peak_usage;
        total.allocation_count += stats_[i].allocation_count;
        total.deallocation_count += stats_[i].deallocation_count;
    }
    return total;
}

void MemoryTracker::reset() {
    std::memset(stats_, 0, sizeof(stats_));
}

// MallocAllocator implementation
MallocAllocator::MallocAllocator(MemoryCategory category)
    : category_(category), total_allocated_(0), allocation_count_(0) {}

void* MallocAllocator::allocate(size_t size, size_t alignment) {
    if (size == 0) return nullptr;

    void* ptr = nullptr;
    if (alignment <= DEFAULT_ALIGNMENT) {
        ptr = std::malloc(size);
    } else {
        ptr = POKO_ALIGNED_ALLOC(align_size(size, alignment), alignment);
    }

    if (ptr) {
        total_allocated_ += size;
        allocation_count_++;
        MemoryTracker::instance().record_allocation(size, category_);
    } else {
        std::ostringstream oss;
        oss << "MallocAllocator failed to allocate " << size << " bytes";
        POKO_LOG_ERROR(oss.str());
    }

    return ptr;
}

void MallocAllocator::free(void* ptr, size_t size) {
    if (!ptr) return;

#ifdef _WIN32
    // Check if this was an aligned allocation
    // We don't track this in MallocAllocator, so just use free
    std::free(ptr);
#else
    std::free(ptr);
#endif

    if (size > 0) {
        total_allocated_ -= size;
        allocation_count_--;
        MemoryTracker::instance().record_deallocation(size, category_);
    }
}

// PoolAllocator implementation
PoolAllocator::PoolAllocator(size_t object_size, size_t object_count, MemoryCategory category)
    : memory_(nullptr)
    , free_list_(nullptr)
    , object_size_(object_size)
    , object_count_(object_count)
    , free_count_(object_count)
    , total_allocated_(0)
    , allocation_count_(0)
    , category_(category) {

    // Align object size to pointer size for free list
    object_size_ = align_size(object_size, sizeof(void*));

    size_t total_size = object_size_ * object_count_;
    memory_ = std::malloc(total_size);

    if (memory_) {
        // Initialize free list
        uint8_t* ptr = static_cast<uint8_t*>(memory_);
        for (size_t i = 0; i < object_count_; ++i) {
            FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
            node->next = (i < object_count_ - 1) ? reinterpret_cast<FreeNode*>(ptr + object_size_) : nullptr;
            ptr += object_size_;
        }
        free_list_ = static_cast<FreeNode*>(memory_);
        total_allocated_ = total_size;
        MemoryTracker::instance().record_allocation(total_size, category_);
    } else {
        std::ostringstream oss;
        oss << "PoolAllocator failed to allocate " << total_size << " bytes";
        POKO_LOG_ERROR(oss.str());
    }
}

PoolAllocator::~PoolAllocator() {
    if (memory_) {
        MemoryTracker::instance().record_deallocation(total_allocated_, category_);
        std::free(memory_);
    }
}

void* PoolAllocator::allocate(size_t size, size_t alignment) {
    if (size > object_size_ || !free_list_) {
        std::ostringstream oss;
        oss << "PoolAllocator: Cannot allocate " << size << " bytes (object size: "
            << object_size_ << ", free: " << free_count_ << ")";
        POKO_LOG_ERROR(oss.str());
        return nullptr;
    }

    if (alignment > object_size_ && !is_aligned(memory_, alignment)) {
        std::ostringstream oss;
        oss << "PoolAllocator: Cannot satisfy alignment " << alignment;
        POKO_LOG_ERROR(oss.str());
        return nullptr;
    }

    FreeNode* node = free_list_;
    free_list_ = free_list_->next;
    free_count_--;
    allocation_count_++;

    return node;
}

void PoolAllocator::free(void* ptr, size_t size) {
    (void)size; // Unused parameter
    if (!ptr) return;

    FreeNode* node = static_cast<FreeNode*>(ptr);
    node->next = free_list_;
    free_list_ = node;
    free_count_++;
    allocation_count_--;
}

void PoolAllocator::reset() {
    if (!memory_) return;

    // Rebuild free list
    uint8_t* ptr = static_cast<uint8_t*>(memory_);
    for (size_t i = 0; i < object_count_; ++i) {
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        node->next = (i < object_count_ - 1) ? reinterpret_cast<FreeNode*>(ptr + object_size_) : nullptr;
        ptr += object_size_;
    }
    free_list_ = static_cast<FreeNode*>(memory_);
    free_count_ = object_count_;
    allocation_count_ = 0;
}

// StackAllocator implementation
StackAllocator::StackAllocator(size_t size, MemoryCategory category)
    : memory_(nullptr)
    , start_ptr_(nullptr)
    , current_ptr_(nullptr)
    , end_ptr_(nullptr)
    , allocation_count_(0)
    , category_(category) {

    memory_ = std::malloc(size);

    if (memory_) {
        start_ptr_ = static_cast<uint8_t*>(memory_);
        current_ptr_ = start_ptr_;
        end_ptr_ = start_ptr_ + size;
        MemoryTracker::instance().record_allocation(size, category_);
    } else {
        std::ostringstream oss;
        oss << "StackAllocator failed to allocate " << size << " bytes";
        POKO_LOG_ERROR(oss.str());
    }
}

StackAllocator::~StackAllocator() {
    if (memory_) {
        MemoryTracker::instance().record_deallocation(current_ptr_ - start_ptr_, category_);
        std::free(memory_);
    }
}

void* StackAllocator::allocate(size_t size, size_t alignment) {
    if (size == 0) return nullptr;

    uint8_t* aligned_ptr = static_cast<uint8_t*>(align_pointer(current_ptr_, alignment));
    uint8_t* header_ptr = aligned_ptr - sizeof(AllocationHeader);

    // Check if we have enough space
    if (header_ptr + sizeof(AllocationHeader) + size > end_ptr_) {
        std::ostringstream oss;
        oss << "StackAllocator: Out of memory (requested: " << size
            << ", remaining: " << (end_ptr_ - current_ptr_) << ")";
        POKO_LOG_ERROR(oss.str());
        return nullptr;
    }

    // Store header for freeing
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(header_ptr);
    header->offset = current_ptr_ - start_ptr_;
    header->alignment = alignment;

    current_ptr_ = aligned_ptr + size;
    allocation_count_++;

    return aligned_ptr;
}

void StackAllocator::free(void* ptr, size_t size) {
    (void)size; // Unused parameter
    if (!ptr) return;

    AllocationHeader* header = static_cast<AllocationHeader*>(ptr) - 1;
    current_ptr_ = start_ptr_ + header->offset;
    allocation_count_--;
}

void StackAllocator::free_to_marker(size_t marker) {
    if (marker <= static_cast<size_t>(current_ptr_ - start_ptr_)) {
        current_ptr_ = start_ptr_ + marker;
    }
}

void StackAllocator::reset() {
    current_ptr_ = start_ptr_;
    allocation_count_ = 0;
}

// Helper functions
void* allocate(size_t size, MemoryCategory category) {
    static MallocAllocator allocator(category);
    return allocator.allocate(size);
}

void free(void* ptr, size_t size, MemoryCategory category) {
    static MallocAllocator allocator(category);
    allocator.free(ptr, size);
}

} // namespace memory
} // namespace core
} // namespace poko
