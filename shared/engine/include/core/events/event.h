/**
 * @file event.h
 * @brief Core events system main header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides the event system for the Poko Engine including
 * event types, signal/slot connections, and event queues for decoupled
 * communication between engine components.
 */

#ifndef POKO_CORE_EVENTS_EVENT_H
#define POKO_CORE_EVENTS_EVENT_H

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

namespace poko {
namespace core {
namespace events {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Unique identifier for event types
 * 
 * Each event type has a unique ID for fast dispatching.
 * IDs are assigned sequentially and can be used for event routing.
 */
using EventId = uint32_t;

/**
 * @brief Priority for event processing
 * 
 * Higher priority events are processed first in the queue.
 */
enum class EventPriority : uint8_t {
    Low = 0,        ///< Low priority (background events)
    Normal = 1,     ///< Normal priority (standard events)
    High = 2,       ///< High priority (important events)
    Critical = 3    ///< Critical priority (immediate processing)
};

// ============================================================================
// Constants
// ============================================================================

/// Invalid event ID (used for null/uninitialized events)
constexpr EventId INVALID_EVENT_ID = 0;

/// Maximum number of event types (adjust based on application needs)
constexpr EventId MAX_EVENT_TYPES = 1024;

// ============================================================================
// Event Base Class
// ============================================================================

/**
 * @brief Base class for all events
 * 
 * All custom events must inherit from this class and provide
 * a unique event ID.
 * 
 * @section thread_safety Thread Safety
 * Events themselves are generally not thread-safe after creation.
 * The event system provides thread-safe dispatching.
 */
class Event {
public:
    /**
     * @brief Constructor
     * @param id Unique event type ID
     * @param priority Event processing priority
     */
    constexpr Event(EventId id, EventPriority priority = EventPriority::Normal) noexcept
        : m_id(id)
        , m_priority(priority)
        , m_timestamp(0)
        , m_handled(false)
    {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Event() = default;
    
    /**
     * @brief Get event type ID
     * @return Event ID
     */
    [[nodiscard]] constexpr EventId getId() const noexcept {
        return m_id;
    }
    
    /**
     * @brief Get event priority
     * @return Event priority
     */
    [[nodiscard]] constexpr EventPriority getPriority() const noexcept {
        return m_priority;
    }
    
    /**
     * @brief Get event timestamp
     * @return Timestamp when event was created
     */
    [[nodiscard]] constexpr uint64_t getTimestamp() const noexcept {
        return m_timestamp;
    }
    
    /**
     * @brief Set event timestamp
     * @param timestamp Timestamp to set
     */
    void setTimestamp(uint64_t timestamp) noexcept {
        m_timestamp = timestamp;
    }
    
    /**
     * @brief Check if event has been handled
     * @return True if event was marked as handled
     */
    [[nodiscard]] constexpr bool isHandled() const noexcept {
        return m_handled;
    }
    
    /**
     * @brief Mark event as handled
     */
    void markHandled() noexcept {
        m_handled = true;
    }
    
    /**
     * @brief Clone event (for queuing)
     * @return Copy of this event
     */
    [[nodiscard]] virtual std::unique_ptr<Event> clone() const = 0;
    
protected:
    EventId m_id;                    ///< Event type ID
    EventPriority m_priority;        ///< Event processing priority
    uint64_t m_timestamp;           ///< Creation timestamp
    bool m_handled;                 ///< Whether event was handled
};

// ============================================================================
// Event Type Registration
// ============================================================================

/**
 * @brief Global event type registry
 * 
 * Assigns unique IDs to event types at startup.
 * Thread-safe ID generation.
 */
class EventTypeRegistry {
public:
    /**
     * @brief Get next available event ID
     * @return Unique event ID
     * 
     * @note Thread-safe
     * @note IDs are assigned sequentially starting from 1
     */
    static EventId getNextId() noexcept;
    
    /**
     * @brief Reset registry (for testing)
     * 
     * @warning Do not call this in production
     */
    static void reset() noexcept;
    
private:
    static std::atomic<EventId> s_nextId;
};

// ============================================================================
// Event Macros
// ============================================================================

/**
 * @brief Declare a new event type
 * 
 * Usage:
 * @code
 * DECLARE_EVENT(MyEvent, 100)
 * class MyEvent : public Event {
 * public:
 *     MyEvent() : Event(EVENT_ID_MyEvent) {}
 *     std::unique_ptr<Event> clone() const override {
 *         return std::make_unique<MyEvent>(*this);
 *     }
 * };
 * @endcode
 */
#define DECLARE_EVENT(ClassName, Id) \
    static constexpr EventId EVENT_ID_##ClassName = Id;

/**
 * @brief Register a new event type with auto-generated ID
 * 
 * Usage:
 * @code
 * REGISTER_EVENT(MyEvent)
 * class MyEvent : public Event {
 * public:
 *     MyEvent() : Event(EVENT_ID_MyEvent) {}
 *     std::unique_ptr<Event> clone() const override {
 *         return std::make_unique<MyEvent>(*this);
 *     }
 * };
 * @endcode
 */
#define REGISTER_EVENT(ClassName) \
    static const EventId EVENT_ID_##ClassName = ::poko::core::events::EventTypeRegistry::getNextId();

// ============================================================================
// Event Callback
// ============================================================================

/**
 * @brief Callback function signature for event handlers
 * 
 * Event handlers receive a pointer to the event and can process it.
 * 
 * @param event Pointer to the event (non-null)
 * @return True if event was handled, false to continue propagation
 */
using EventCallback = std::function<bool(Event*)>;

// ============================================================================
// Signal
// ============================================================================

/**
 * @brief Signal for event broadcasting
 * 
 * Signals allow multiple listeners (slots) to subscribe to events.
 * When a signal is emitted, all connected callbacks are invoked.
 * 
 * @section thread_safety Thread Safety
 * - Connecting/disconnecting is thread-safe
 * - Emitting is thread-safe
 * - Callbacks are invoked in the order they were connected
 * 
 * @section performance Performance
 * - Connection: O(1)
 * - Disconnection: O(n) where n is number of connections
 * - Emission: O(n) where n is number of connections
 */
class Signal {
public:
    /**
     * @brief Constructor
     */
    Signal() noexcept = default;
    
    /**
     * @brief Destructor
     */
    ~Signal() = default;
    
    /**
     * @brief Copy constructor (deleted)
     */
    Signal(const Signal&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    Signal& operator=(const Signal&) = delete;
    
    /**
     * @brief Move constructor
     */
    Signal(Signal&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    Signal& operator=(Signal&&) noexcept = default;
    
    /**
     * @brief Connect a callback to this signal
     * 
     * @param callback Function to call when signal is emitted
     * @return Connection ID (used for disconnection)
     * 
     * @note Thread-safe
     * @note Same callback can be connected multiple times
     */
    [[nodiscard]] size_t connect(EventCallback callback);
    
    /**
     * @brief Disconnect a callback by connection ID
     * 
     * @param connectionId ID returned by connect()
     * @return True if connection was found and removed
     * 
     * @note Thread-safe
     */
    bool disconnect(size_t connectionId);
    
    /**
     * @brief Disconnect all callbacks
     * 
     * @note Thread-safe
     */
    void disconnectAll();
    
    /**
     * @brief Emit signal with event
     * 
     * @param event Event to emit
     * @return Number of handlers that handled the event
     * 
     * @note Thread-safe
     * @note Event is not modified by the signal
     */
    size_t emit(Event* event);
    
    /**
     * @brief Get number of connected callbacks
     * @return Connection count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getConnectionCount() const;
    
private:
    struct Connection {
        size_t id;
        EventCallback callback;
    };
    
    mutable std::mutex m_mutex;
    std::vector<Connection> m_connections;
    std::atomic<size_t> m_nextConnectionId{1};
};

// ============================================================================
// Event Queue
// ============================================================================

/**
 * @brief Thread-safe event queue for deferred processing
 * 
 * Events can be queued for later processing, allowing for:
 * - Main thread processing of background events
 * - Event prioritization
 * - Thread-safe event passing
 * 
 * @section thread_safety Thread Safety
 * - All operations are thread-safe
 * - Uses mutex for synchronization
 * 
 * @section performance Performance
 * - Push: O(1) amortized
 * - Pop: O(n) due to priority sorting
 * - Size: O(1)
 */
class EventQueue {
public:
    /**
     * @brief Constructor
     */
    EventQueue() noexcept = default;
    
    /**
     * @brief Destructor
     */
    ~EventQueue() = default;
    
    /**
     * @brief Copy constructor (deleted)
     */
    EventQueue(const EventQueue&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    EventQueue& operator=(const EventQueue&) = delete;
    
    /**
     * @brief Move constructor
     */
    EventQueue(EventQueue&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    EventQueue& operator=(EventQueue&&) noexcept = default;
    
    /**
     * @brief Push event to queue
     * 
     * @param event Event to queue (takes ownership)
     * 
     * @note Thread-safe
     * @note Events are automatically sorted by priority
     */
    void push(std::unique_ptr<Event> event);
    
    /**
     * @brief Pop next event from queue
     * 
     * @return Next event, or nullptr if queue is empty
     * 
     * @note Thread-safe
     * @note Returns highest priority event first
     */
    [[nodiscard]] std::unique_ptr<Event> pop();
    
    /**
     * @brief Check if queue is empty
     * @return True if queue has no events
     * 
     * @note Thread-safe
     */
    [[nodiscard]] bool empty() const;
    
    /**
     * @brief Get queue size
     * @return Number of events in queue
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t size() const;
    
    /**
     * @brief Clear all events from queue
     * 
     * @note Thread-safe
     */
    void clear();
    
private:
    mutable std::mutex m_mutex;
    std::vector<std::unique_ptr<Event>> m_events;
};

// ============================================================================
// Event Dispatcher
// ============================================================================

/**
 * @brief Central event dispatcher
 * 
 * Manages event routing to registered handlers.
 * Supports both immediate dispatch and queuing.
 * 
 * @section thread_safety Thread Safety
 * - All operations are thread-safe
 * - Handler registration is thread-safe
 * - Dispatch is thread-safe
 * 
 * @section performance Performance
 * - Handler registration: O(1)
 * - Dispatch: O(n) where n is number of handlers for event type
 * - Queued dispatch: O(1) to queue, O(n) to process
 */
class EventDispatcher {
public:
    /**
     * @brief Constructor
     */
    EventDispatcher() noexcept = default;
    
    /**
     * @brief Destructor
     */
    ~EventDispatcher() = default;
    
    /**
     * @brief Copy constructor (deleted)
     */
    EventDispatcher(const EventDispatcher&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    EventDispatcher& operator=(const EventDispatcher&) = delete;
    
    /**
     * @brief Move constructor
     */
    EventDispatcher(EventDispatcher&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    EventDispatcher& operator=(EventDispatcher&&) noexcept = default;
    
    /**
     * @brief Register handler for event type
     * 
     * @param eventId Event type ID to handle
     * @param callback Handler callback
     * @return Connection ID
     * 
     * @note Thread-safe
     * @note Multiple handlers can be registered for same event type
     */
    [[nodiscard]] size_t registerHandler(EventId eventId, EventCallback callback);
    
    /**
     * @brief Unregister handler by connection ID
     * 
     * @param connectionId ID returned by registerHandler()
     * @return True if handler was found and removed
     * 
     * @note Thread-safe
     */
    bool unregisterHandler(size_t connectionId);
    
    /**
     * @brief Dispatch event immediately
     * 
     * @param event Event to dispatch
     * @return Number of handlers that handled the event
     * 
     * @note Thread-safe
     * @note Event is processed synchronously
     */
    size_t dispatch(Event* event);
    
    /**
     * @brief Queue event for later processing
     * 
     * @param event Event to queue (takes ownership)
     * 
     * @note Thread-safe
     * @note Event will be processed when processQueue() is called
     */
    void queue(std::unique_ptr<Event> event);
    
    /**
     * @brief Process all queued events
     * 
     * @return Number of events processed
     * 
     * @note Thread-safe
     * @note Events are processed in priority order
     */
    size_t processQueue();
    
    /**
     * @brief Get event queue for direct access
     * @return Reference to internal event queue
     * 
     * @note Thread-safe access to queue operations
     */
    [[nodiscard]] EventQueue& getQueue();
    
    /**
     * @brief Clear all queued events
     * 
     * @note Thread-safe
     */
    void clearQueue();
    
private:
    struct HandlerInfo {
        size_t connectionId;
        EventId eventId;
        EventCallback callback;
    };
    
    mutable std::mutex m_mutex;
    std::vector<HandlerInfo> m_handlers;
    std::atomic<size_t> m_nextConnectionId{1};
    EventQueue m_queue;
};

// ============================================================================
// Global Event Dispatcher
// ============================================================================

/**
 * @brief Global event dispatcher instance
 * 
 * Points to the globally shared EventDispatcher instance.
 * Used throughout the engine for event routing.
 */
extern EventDispatcher* g_eventDispatcher;

/**
 * @brief Get global event dispatcher
 * 
 * Returns a reference to the globally shared EventDispatcher instance.
 * This dispatcher is intended for general-purpose event handling throughout the engine.
 * 
 * @return Reference to global event dispatcher
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this dispatcher - it's globally managed
 */
EventDispatcher& getEventDispatcher();

} // namespace events
} // namespace core
} // namespace poko

#endif // POKO_CORE_EVENTS_EVENT_H
