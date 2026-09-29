# Core Events and Signals

## Overview

The Core Events and Signals module provides a thread-safe event system with signal/slot pattern for decoupled communication between engine components.

## Features

### Event System
- Base Event class with type registry
- Event priorities (Low, Normal, High, Critical)
- Event cloning support
- Type-safe event handling

### Signal/Slot Pattern
- Decoupled message passing
- Connection/disconnection with unique IDs
- Multiple connections per signal
- Disconnect all connections at once

### Event Queues
- Priority-ordered event queues
- Push/pop operations
- Queue clearing
- Thread-safe operations

### Event Dispatcher
- Type-safe event registration
- Event dispatching to registered handlers
- Queued dispatch mode
- Multiple event type support

### Thread Safety
- Mutex-protected operations
- Thread-safe global dispatcher
- Safe for concurrent event emission

## API

### Event

```cpp
class Event {
public:
    virtual ~Event() = default;
    virtual Event* clone() const = 0;
    virtual uint32_t getTypeId() const = 0;
    EventPriority getPriority() const;
    void setPriority(EventPriority priority);
};
```

### Signal

```cpp
class Signal {
public:
    ConnectionId connect(EventHandler handler);
    void disconnect(ConnectionId connectionId);
    void disconnectAll();
    void emit(const Event& event);
};
```

### EventQueue

```cpp
class EventQueue {
public:
    void push(const Event& event);
    std::unique_ptr<Event> pop();
    void clear();
    size_t size() const;
    bool isEmpty() const;
};
```

### EventDispatcher

```cpp
class EventDispatcher {
public:
    void registerHandler(uint32_t eventTypeId, EventHandler handler);
    void unregisterHandler(uint32_t eventTypeId, ConnectionId connectionId);
    void dispatch(const Event& event);
    void queue(const Event& event);
    void processQueue();
};
```

### Global Access

```cpp
EventDispatcher& getGlobalEventDispatcher();
```

## Usage Example

```cpp
#include "core/events/event.h"

using namespace poko::core::events;

// Get global dispatcher
EventDispatcher& dispatcher = getGlobalEventDispatcher();

// Register handler
ConnectionId conn = dispatcher.registerHandler(MyEvent::getTypeId(), 
    [](const Event& event) {
        // Handle event
    });

// Emit event
MyEvent myEvent;
dispatcher.dispatch(myEvent);

// Disconnect handler
dispatcher.unregisterHandler(MyEvent::getTypeId(), conn);
```

## Fine-Grained Translation Units

The Core Events module is split into 11 separate source files for fast incremental builds:

- `interface/interface.cpp` - Global event dispatcher
- `registry/registry.cpp` - Event type registry
- `signal/connect.cpp` - Signal connection
- `signal/disconnect.cpp` - Signal disconnection
- `signal/emit.cpp` - Signal emission
- `queue/push.cpp` - Queue push operation
- `queue/pop.cpp` - Queue pop operation
- `queue/info.cpp` - Queue info operations
- `dispatcher/register.cpp` - Dispatcher registration
- `dispatcher/unregister.cpp` - Dispatcher unregistration
- `dispatcher/dispatch.cpp` - Dispatcher dispatch
- `dispatcher/queue.cpp` - Dispatcher queue processing

## Building

```bash
cd shared/engine
cmake -B build
cmake --build build
```

## Testing

```bash
cd build
./test_event.exe
```

## Notes

- All operations are thread-safe
- Priority queues ensure high-priority events are processed first
- Connection IDs enable selective disconnection
- Event cloning enables safe event copying
- Supports both immediate and queued dispatch modes
