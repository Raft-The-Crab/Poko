/**
 * @file websocket_client.h
 * @brief WebSocket client interface for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <string>
#include <functional>
#include <memory>
#include <libwebsockets.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <cstdint>

namespace Poko {
namespace Networking {

/**
 * @brief WebSocket connection state
 */
enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
    Error
};

/**
 * @brief WebSocket message callback type
 */
using MessageCallback = std::function<void(const std::string& message)>;
using BinaryMessageCallback = std::function<void(const std::vector<uint8_t>& data)>;
using ConnectionCallback = std::function<void(ConnectionState state)>;
using ErrorCallback = std::function<void(const std::string& error)>;

/**
 * @brief WebSocket client configuration
 */
struct WebSocketConfig {
    std::string url = "ws://localhost:8080";
    std::string protocol = "http";
    int timeout_seconds = 30;
    int reconnect_delay_ms = 1000;
    int max_reconnect_attempts = 5;
    bool auto_reconnect = true;
    size_t max_queue_size = 1000;
    bool enable_compression = false;
    int heartbeat_interval_ms = 30000; // 30 seconds
    int connection_timeout_ms = 10000; // 10 seconds
};

/**
 * @brief WebSocket client for real-time networking
 */
class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    /**
     * @brief Initialize the WebSocket client
     * @param config Client configuration
     * @return true if initialization succeeded
     */
    bool Initialize(const WebSocketConfig& config);

    /**
     * @brief Shutdown the WebSocket client
     */
    void Shutdown();

    /**
     * @brief Connect to the WebSocket server
     * @return true if connection initiated
     */
    bool Connect();

    /**
     * @brief Disconnect from the WebSocket server
     */
    void Disconnect();

    /**
     * @brief Send a message
     * @param message Message to send
     * @return true if message queued for sending
     */
    bool SendMessage(const std::string& message);

    /**
     * @brief Send a message with priority
     * @param message Message to send
     * @param priority Message priority (higher = sent first)
     * @return true if message queued for sending
     */
    bool SendMessage(const std::string& message, int priority);

    /**
     * @brief Send binary data
     * @param data Binary data to send
     * @return true if data queued for sending
     */
    bool SendBinary(const std::vector<uint8_t>& data);

    /**
     * @brief Send binary data with priority
     * @param data Binary data to send
     * @param priority Message priority (higher = sent first)
     * @return true if data queued for sending
     */
    bool SendBinary(const std::vector<uint8_t>& data, int priority);

    /**
     * @brief Set message callback
     * @param callback Function to call when message received
     */
    void SetMessageCallback(MessageCallback callback);

    /**
     * @brief Set binary message callback
     * @param callback Function to call when binary data received
     */
    void SetBinaryMessageCallback(BinaryMessageCallback callback);

    /**
     * @brief Set connection state callback
     * @param callback Function to call when connection state changes
     */
    void SetConnectionCallback(ConnectionCallback callback);

    /**
     * @brief Set error callback
     * @param callback Function to call when error occurs
     */
    void SetErrorCallback(ErrorCallback callback);

    /**
     * @brief Update WebSocket client (called each frame)
     * @param delta_time Time since last update
     */
    void Update(float delta_time);

    /**
     * @brief Get current connection state
     * @return Current connection state
     */
    ConnectionState GetConnectionState() const;

    /**
     * @brief Check if initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

    /**
     * @brief Get number of queued messages
     * @return Number of messages waiting to be sent
     */
    size_t GetQueuedMessageCount() const;

    /**
     * @brief Clear all queued messages
     */
    void ClearQueuedMessages();

    /**
     * @brief Send heartbeat/ping message
     * @return true if heartbeat sent
     */
    bool SendHeartbeat();

    /**
     * @brief Get connection statistics
     * @return Connection statistics as string
     */
    std::string GetConnectionStats() const;

private:
    void ReconnectThread();
    bool PerformConnection();
    void ProcessOutgoingMessages();
    void SetConnectionState(ConnectionState state);
    void HandleError(const std::string& error);
    bool ParseURL(const std::string& url);

    // libwebsockets callback handlers
    void OnConnectionEstablished();
    void OnMessageReceived(const std::string& message);
    void OnBinaryMessageReceived(const std::vector<uint8_t>& data);
    void OnConnectionClosed();
    void OnConnectionError(const std::string& error);
    void OnWritable();

    std::atomic<bool> m_initialized;
    std::atomic<bool> m_should_stop;
    WebSocketConfig m_config;
    std::atomic<ConnectionState> m_connection_state;
    
    // Callbacks (thread-safe)
    MessageCallback m_message_callback;
    BinaryMessageCallback m_binary_message_callback;
    ConnectionCallback m_connection_callback;
    ErrorCallback m_error_callback;
    mutable std::mutex m_callback_mutex;

    // libwebsockets context
    struct lws_context* m_context;
    struct lws* m_wsi;

    // Message queue
    struct QueuedMessage {
        std::string data;
        std::vector<uint8_t> binary_data;
        bool is_binary;
        int priority;
        bool operator<(const QueuedMessage& other) const {
            return priority < other.priority;
        }
    };
    std::priority_queue<QueuedMessage> m_outgoing_queue;
    mutable std::mutex m_queue_mutex;
    std::condition_variable m_write_condition;

    // Reconnection
    std::thread m_reconnect_thread;
    std::atomic<int> m_reconnect_attempts;
    std::atomic<bool> m_should_reconnect;

    // URL parsing
    std::string m_current_host;
    int32_t m_current_port;
    std::string m_current_path;
    bool m_use_ssl;

    // Heartbeat
    std::chrono::steady_clock::time_point m_last_heartbeat;
    std::chrono::steady_clock::time_point m_last_pong;
    std::atomic<uint64_t> m_messages_sent;
    std::atomic<uint64_t> m_messages_received;
    std::atomic<uint64_t> m_bytes_sent;
    std::atomic<uint64_t> m_bytes_received;

    // Allow callback to access private members
    friend int websocket_callback(struct lws* wsi, enum lws_callback_reasons reason,
                                  void* user, void* in, size_t len);
};

} // namespace Networking
} // namespace Poko