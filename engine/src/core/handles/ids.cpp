/**
 * @file ids.cpp
 * @brief Implementation of core ID types and handles
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/handles/ids.h"
#include <mutex>
#include <cstring>

namespace poko {
namespace core {

// IDGenerator implementation
IDGenerator::IDGenerator(uint64_t start)
    : next_id_(start), max_id_(start - 1) {}

uint64_t IDGenerator::generate() {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    return next_id_++;
}

bool IDGenerator::reserve(uint64_t id) {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    if (id >= next_id_) {
        next_id_ = id + 1;
        max_id_ = id;
        return true;
    }
    return false;
}

void IDGenerator::reset(uint64_t start) {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    next_id_ = start;
    max_id_ = start - 1;
}

// HandleTable implementation
template<typename IDType>
HandleTable<IDType>::HandleTable()
    : active_count_(0), next_free_(0) {
    std::memset(entries_, 0, sizeof(entries_));
}

template<typename IDType>
Handle<IDType> HandleTable<IDType>::allocate(IDType id) {
    if (next_free_ >= MAX_HANDLES) {
        return Handle<IDType>(); // Table full
    }

    size_t index = next_free_;
    entries_[index].id = id;
    entries_[index].generation++;
    entries_[index].active = true;

    // Find next free slot
    next_free_ = MAX_HANDLES;
    for (size_t i = index + 1; i < MAX_HANDLES; ++i) {
        if (!entries_[i].active) {
            next_free_ = i;
            break;
        }
    }

    active_count_++;
    return Handle<IDType>(id, entries_[index].generation);
}

template<typename IDType>
bool HandleTable<IDType>::deallocate(const Handle<IDType>& handle) {
    if (!handle.is_valid()) {
        return false;
    }

    // Find the entry
    for (size_t i = 0; i < MAX_HANDLES; ++i) {
        if (entries_[i].active && entries_[i].id == handle.id()) {
            if (entries_[i].generation != handle.generation()) {
                return false; // Generation mismatch
            }

            entries_[i].active = false;
            if (i < next_free_) {
                next_free_ = i;
            }
            active_count_--;
            return true;
        }
    }

    return false; // Not found
}

template<typename IDType>
bool HandleTable<IDType>::is_valid(const Handle<IDType>& handle) const {
    if (!handle.is_valid()) {
        return false;
    }

    for (size_t i = 0; i < MAX_HANDLES; ++i) {
        if (entries_[i].active && entries_[i].id == handle.id()) {
            return entries_[i].generation == handle.generation();
        }
    }

    return false;
}

template<typename IDType>
void HandleTable<IDType>::clear() {
    std::memset(entries_, 0, sizeof(entries_));
    active_count_ = 0;
    next_free_ = 0;
}

// Explicit template instantiations
template class HandleTable<InstanceIDUnderlying>;
template class HandleTable<uint32_t>; // For ResourceID, AssetID, ComponentID (all uint32_t)

} // namespace core
} // namespace poko
