# Core Logging

## Overview

The Core Logging module provides a thread-safe logging system with multiple sinks, severity levels, and formatted output for engine-wide logging needs.

## Features

### Severity Levels
- Debug, Info, Warning, Error, Fatal
- Level filtering (only log at or above minimum level)
- Level-to-string conversion
- String-to-level conversion

### Multiple Sinks
- Console sink for immediate output
- File sink for persistent logging
- Extensible sink interface
- Multiple sinks per logger

### Formatting
- Timestamps with millisecond precision
- Subsystem/category filtering
- Source file, line, and function context
- Structured log message format

### Thread Safety
- Mutex-protected operations
- Safe for concurrent logging
- Thread-safe global logger

### Convenience Macros
- LOG_DEBUG(subsystem, message)
- LOG_INFO(subsystem, message)
- LOG_WARNING(subsystem, message)
- LOG_ERROR(subsystem, message)
- LOG_FATAL(subsystem, message)

## API

### LogLevel

```cpp
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

const char* logLevelToString(LogLevel level);
LogLevel stringToLogLevel(const std::string& str);
```

### LogSink

```cpp
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogMessage& message) = 0;
    virtual void flush() = 0;
};
```

### ConsoleSink

```cpp
class ConsoleSink : public LogSink {
public:
    void write(const LogMessage& message) override;
    void flush() override;
};
```

### FileSink

```cpp
class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& filepath);
    ~FileSink() override;
    
    void write(const LogMessage& message) override;
    void flush() override;
};
```

### Logger

```cpp
class Logger {
public:
    Logger();
    ~Logger();
    
    void log(LogLevel level, const std::string& subsystem, const std::string& message,
             const std::string& file = "", int line = 0, const std::string& function = "");
    
    void addSink(std::shared_ptr<LogSink> sink);
    void removeSink(LogSink* sink);
    
    void setMinLevel(LogLevel level);
    LogLevel getMinLevel() const;
    
    void flush();
    
    void setSubsystemFilter(const std::string& subsystem);
    std::string getSubsystemFilter() const;
};
```

### Global Access

```cpp
Logger& getGlobalLogger();
```

## Usage Example

```cpp
#include "core/logging/logger.h"

using namespace poko::core::logging;

// Get global logger
Logger& logger = getGlobalLogger();

// Set minimum level
logger.setMinLevel(LogLevel::Debug);

// Log messages
logger.log(LogLevel::Info, "network", "Connected to server");
logger.log(LogLevel::Error, "network", "Connection failed", __FILE__, __LINE__, __FUNCTION__);

// Add file sink
logger.addSink(std::make_shared<FileSink>("engine.log"));

// Use convenience macros
LOG_INFO("render", "Frame rendered in 16ms");
LOG_ERROR("physics", "Simulation failed");
LOG_WARNING("audio", "Audio device not found");

// Flush all sinks
logger.flush();
```

## Log Format

Log messages are formatted as:

```
[timestamp] [level] [subsystem] [file:line] message
```

Example:

```
[2026-09-29 18:45:32.123] [INFO] [network] [network.cpp:145] Connected to server
[2026-09-29 18:45:33.456] [ERROR] [physics] [physics.cpp:234] Simulation failed
```

## Fine-Grained Translation Units

The Core Logging module is split into 11 separate source files for fast incremental builds:

- `utility/utility.cpp` - Log level conversion
- `interface/interface.cpp` - Global logger instance
- `constructor/constructor.cpp` - Constructor/destructor
- `log/log.cpp` - Log operation
- `sink/sink.cpp` - Sink management
- `level/level.cpp` - Level management
- `filter/filter.cpp` - Subsystem filter
- `flush/flush.cpp` - Flush operation
- `format/format.cpp` - Formatting and timestamps
- `console/console.cpp` - Console sink
- `file/file.cpp` - File sink

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_logging.exe
```

## Notes

- All operations are thread-safe
- Console sink writes to stdout
- File sink appends to files
- Timestamps include milliseconds
- Subsystem filtering is case-sensitive
- Level filtering is inclusive (at or above minimum level)
