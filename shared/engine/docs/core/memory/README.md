# Core Memory

## Overview

The Core Memory module provides a production-ready memory management system with thread-safe allocators, alignment support, statistics tracking, and overflow protection.

## Features

### Thread-Safe Allocator
- Mutex-protected operations for concurrent access
- Atomic statistics counters for lock-free reads
- Configurable alignment (default 16 bytes)
- Minimum alignment enforcement (sizeof(void*))

### Memory Protection
- Overflow protection with MAX_ALLOCATION_SIZE validation
- Guard bytes for buffer overflow/underflow detection
- No-throw mode for graceful error handling
- Null deallocation safety

### Statistics Tracking
- Current memory usage
- Peak memory usage
- Total allocations
- Total deallocations
- Allocation count tracking

### Global Access
- Thread-safe global system allocator
- RAII-based memory management
- Cross-platform support (Windows/Linux)

## API

### IAllocator Interface

```cpp
class IAllocator {
public:
    virtual void* allocate(size_t size, size_t alignment) = 0;
    virtual void deallocate(void* ptr, size_t alignment) = 0;
    virtual AllocationStats getStats() const = 0;
    virtual void reset() = 0;
    virtual void setTrackingEnabled(bool enabled) = 0;
    virtual bool isTrackingEnabled() const = 0;
};
```

### SystemAllocator

```cpp
class SystemAllocator : public IAllocator {
public:
    SystemAllocator();
    ~SystemAllocator();
    
    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* ptr, size_t alignment) override;
    AllocationStats getStats() const override;
    void reset() override;
    void setTrackingEnabled(bool enabled) override;
    bool isTrackingEnabled() const override;
};
```

### Global Access

```cpp
IAllocator& getSystemAllocator();
```

## Usage Example

```cpp
#include "core/memory/allocator.h"

using namespace poko::core::memory;

// Get global allocator
IAllocator& allocator = getSystemAllocator();

// Allocate memory
void* ptr = allocator.allocate(1024, 16);

// Use memory
// ...

// Deallocate memory
allocator.deallocate(ptr, 16);
```

## Fine-Grained Translation Units

The Core Memory module is split into 10 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global system allocator instance
- `system/constructor.cpp` - Constructor/destructor
- `system/allocate.cpp` - Allocation logic
- `system/deallocate.cpp` - Deallocation logic
- `system/stats.cpp` - Statistics tracking
- `system/reset.cpp` - Reset functionality
- `system/getter.cpp` - Global getter
- `system/info.cpp` - Info methods
- `system/tracking.cpp` - Tracking flag management

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_allocator.exe
```

## Notes

- All operations are thread-safe
- Uses platform-specific memory allocation (_aligned_malloc on Windows, aligned_alloc on POSIX)
- Guard bytes are 8 bytes before and after allocations
- Statistics use atomic operations for lock-free reads
- No-throw mode returns nullptr instead of throwing exceptions
