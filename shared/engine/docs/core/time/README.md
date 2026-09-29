# Core Time

## Overview

The Core Time module provides high-resolution timing utilities with nanosecond precision, supporting both time points (moments in time) and durations (time intervals).

## Features

### High-Resolution Timing
- Nanosecond precision using int64_t internally
- Factory methods for unambiguous construction
- Conversion between time units
- Cross-platform support (Windows/POSIX)

### Duration Abstraction
- Time intervals between time points
- Factory methods (fromSeconds, fromMilliseconds, fromMicroseconds, fromNanoseconds)
- Conversion methods (toSeconds, toMilliseconds, toMicroseconds, toNanoseconds)
- State checks (isZero, isPositive, isNegative)
- Comparison and arithmetic operations

### TimePoint Abstraction
- Moments in time
- Factory methods for construction
- Arithmetic with durations
- Comparison operations
- Difference between time points

### Platform Integration
- Current time retrieval
- Sleep functionality
- Elapsed time calculation
- Platform-specific implementations

## API

### Duration

```cpp
struct Duration {
    int64_t nanos;
    
    static Duration fromSeconds(double seconds);
    static Duration fromMilliseconds(double ms);
    static Duration fromMicroseconds(int64_t us);
    static Duration fromNanoseconds(int64_t ns);
    
    double toSeconds() const;
    double toMilliseconds() const;
    int64_t toMicroseconds() const;
    int64_t toNanoseconds() const;
    
    bool isZero() const;
    bool isPositive() const;
    bool isNegative() const;
    
    bool operator==(const Duration& other) const;
    bool operator!=(const Duration& other) const;
    bool operator<(const Duration& other) const;
    Duration operator+(const Duration& other) const;
    Duration operator-(const Duration& other) const;
    Duration operator*(double scalar) const;
    Duration operator/(double scalar) const;
};
```

### TimePoint

```cpp
struct TimePoint {
    int64_t nanos;
    
    static TimePoint fromNanoseconds(int64_t ns);
    static TimePoint fromSeconds(double seconds);
    static TimePoint fromMilliseconds(double ms);
    
    double toSeconds() const;
    double toMilliseconds() const;
    int64_t toMicroseconds() const;
    int64_t toNanoseconds() const;
    
    bool operator==(const TimePoint& other) const;
    bool operator!=(const TimePoint& other) const;
    bool operator<(const TimePoint& other) const;
    TimePoint operator+(const Duration& duration) const;
    Duration operator-(const TimePoint& other) const;
};
```

### Global Functions

```cpp
TimePoint getCurrentTime();
void sleep(Duration duration);
Duration getElapsedTime(TimePoint start);
```

## Usage Example

```cpp
#include "core/time/time.h"

using namespace poko::core::time;

// Get current time
TimePoint start = getCurrentTime();

// Do work
// ...

// Get elapsed time
Duration elapsed = getElapsedTime(start);
std::cout << "Elapsed: " << elapsed.toMilliseconds() << " ms" << std::endl;

// Sleep for 100ms
sleep(Duration::fromMilliseconds(100));
```

## Fine-Grained Translation Units

The Core Time module is split into 3 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global time manager
- `current/current.cpp` - Current time retrieval
- `sleep/sleep.cpp` - Sleep functionality
- `elapsed/elapsed.cpp` - Elapsed time calculation

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_time.exe
```

## Notes

- Uses QueryPerformanceCounter on Windows for high precision
- Uses clock_gettime on POSIX systems
- Factory methods prevent constructor ambiguity
- Nanosecond internal representation for maximum precision
