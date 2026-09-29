# Core Handles and IDs

## Overview

The Core Handles and IDs module provides a generation-aware handle system that prevents stale references and enables safe object references throughout the engine.

## Features

### Generation-Aware Handles
- Index and generation components
- Stale reference prevention using generations
- Generation increment on reuse
- Null/invalid handle detection

### Thread-Safe Operations
- Mutex-protected allocation and deallocation
- Atomic statistics counters
- Free list for index reuse
- Capacity management (1 million handles max)

### Type Safety
- Strongly typed handles
- Handle hashing for unordered containers
- Equality and comparison operators
- Safe validation checks

### Statistics Tracking
- Active handle count
- Total allocated handles
- Total freed handles
- Current capacity

## API

### Handle Structure

```cpp
struct Handle {
    uint32_t index;
    uint32_t generation;
    
    bool isNull() const;
    bool isValid() const;
    bool operator==(const Handle& other) const;
    bool operator!=(const Handle& other) const;
    bool operator<(const Handle& other) const;
};
```

### HandleManager

```cpp
class HandleManager {
public:
    HandleManager();
    ~HandleManager();
    
    Handle allocate();
    void free(Handle handle);
    bool isValid(Handle handle) const;
    uint32_t getGeneration(Handle handle) const;
    HandleStats getStats() const;
    void reset();
    size_t getCapacity() const;
    size_t getActiveCount() const;
};
```

### Global Access

```cpp
HandleManager& getGlobalHandleManager();
```

## Usage Example

```cpp
#include "core/handles/handle_manager.h"

using namespace poko::core::handles;

// Get global handle manager
HandleManager& manager = getGlobalHandleManager();

// Allocate a handle
Handle handle = manager.allocate();

// Validate handle
if (manager.isValid(handle)) {
    // Use handle
    // ...
}

// Free handle
manager.free(handle);
```

## Fine-Grained Translation Units

The Core Handles module is split into 6 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global handle manager instance
- `manager/constructor.cpp` - Constructor/destructor
- `manager/allocate.cpp` - Allocation logic
- `manager/free.cpp` - Free logic
- `manager/validate.cpp` - Validation logic
- `manager/stats.cpp` - Statistics tracking
- `manager/reset.cpp` - Reset functionality

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_handle.exe
```

## Notes

- All operations are thread-safe
- Generation prevents use-after-free bugs
- Free list enables efficient index reuse
- Maximum capacity of 1 million handles
- Handles are hashable for use with unordered containers
