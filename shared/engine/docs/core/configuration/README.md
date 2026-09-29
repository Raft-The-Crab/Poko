# Core Configuration

## Overview

The Core Configuration module provides a thread-safe configuration management system with key-value storage, file I/O, and default value support.

## Features

### Key-Value Storage
- Thread-safe map with variant types (bool, int, double, string)
- Type-safe access methods
- Template-based default value retrieval
- Remove and has operations

### File I/O
- Load configuration from key=value format files
- Save configuration to files
- Comment support (# for comments)
- Automatic type inference

### Default Values
- Fallback values when keys don't exist
- Separate from actual values
- Override defaults with actual values
- Clear default separation

### Filtering
- Level filtering (minimum log level)
- Subsystem filtering
- Get all keys operation
- Size tracking

### Thread Safety
- Mutex-protected operations
- Safe for concurrent access
- Atomic operations where applicable

## API

### Configuration

```cpp
class Configuration {
public:
    Configuration();
    ~Configuration();
    
    void set(const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> get(const std::string& key) const;
    template<typename T> T getOrDefault(const std::string& key, const T& defaultValue) const;
    
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    
    bool has(const std::string& key) const;
    bool remove(const std::string& key);
    void clear();
    size_t size() const;
    
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    void setDefault(const std::string& key, const ConfigValue& value);
    std::vector<std::string> getAllKeys() const;
};
```

### Global Access

```cpp
Configuration& getGlobalConfiguration();
```

## Usage Example

```cpp
#include "core/configuration/configuration.h"

using namespace poko::core::configuration;

// Get global configuration
Configuration& config = getGlobalConfiguration();

// Set values
config.set("window_width", 1920);
config.set("window_height", 1080);
config.set("vsync", true);
config.set("log_level", "INFO");

// Get values
int width = config.getOrDefault<int>("window_width", 1280);
bool vsync = config.getOrDefault<bool>("vsync", false);

// Load from file
config.loadFromFile("config.txt");

// Save to file
config.saveToFile("config.txt");
```

## File Format

Configuration files use a simple key=value format:

```
# Window settings
window_width=1920
window_height=1080
vsync=true

# Logging
log_level=INFO
log_file=engine.log
```

## Fine-Grained Translation Units

The Core Configuration module is split into 10 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global configuration instance
- `constructor/constructor.cpp` - Constructor/destructor
- `set/set.cpp` - Set operation
- `get/get.cpp` - Get operations
- `remove/remove.cpp` - Remove and has operations
- `clear/clear.cpp` - Clear and size operations
- `load/load.cpp` - Load from file
- `save/save.cpp` - Save to file
- `default/default.cpp` - Default value operations
- `keys/keys.cpp` - Get all keys operation

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_configuration.exe
```

## Notes

- All operations are thread-safe
- File format is human-readable and editable
- Automatic type inference from strings
- Comments are ignored during parsing
- Default values are not saved to files
