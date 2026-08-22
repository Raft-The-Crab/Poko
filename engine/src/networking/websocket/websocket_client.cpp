/**
 * @file websocket_client.cpp
 * @brief WebSocket client implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "networking/websocket/websocket_client.h"
#include "core/logging/logger.h"
#include <libwebsockets.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

namespace Poko {
namespace Networking {

// User data structure for libwebsockets
struct PerSessionData {
    WebSocketClient* client;
    std::string buffer;
};

// libwebsockets protocol
static struct lws_protocols websocket_protocols[] = {
    {
        "poko-protocol",
        nullptr,
        sizeof(PerSessionData),
        4096,
        0,
        nullptr,
        0
    },
    { nullptr, nullptr, 0, 0, 0, nullptr, 0 }
};

// libwebsockets callback
int websocket_callback(struct lws* wsi, enum lws_callback_reasons reason,
                       void* user, void* in, size_t len)
{
    (void)wsi;
    PerSessionData* psd = static_cast<PerSessionData*>(user);
    WebSocketClient* client = nullptr;

    if (psd) {
        client = psd->client;
    }

    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            if (client) {
                client->OnConnectionEstablished();
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (client && in && len > 0) {
                // Process received data
                char* data = static_cast<char*>(in);
                std::string message(data, len);
                client->OnMessageReceived(message);
            }
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            if (client) {
                client->OnConnectionClosed();
            }
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            if (client && in) {
                char* error = static_cast<char*>(in);
                std::string error_msg(error, len);
                client->OnConnectionError(error_msg);
            } else if (client) {
                client->OnConnectionError("Unknown connection error");
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if (client) {
                client->OnWritable();
            }
            break;

        default:
            break;
    }

    return 0;
}

// Update the protocols array to use our callback
void init_protocols()
{
    websocket_protocols[0].callback = &websocket_callback;
}

// Static initializer
struct ProtocolInitializer {
    ProtocolInitializer() {
        init_protocols();
    }
};
static ProtocolInitializer g_protocol_init;

WebSocketClient::WebSocketClient()
    : m_initialized(false)
    , m_should_stop(false)
    , m_connection_state(ConnectionState::Disconnected)
    , m_context(nullptr)
    , m_wsi(nullptr)
    , m_reconnect_attempts(0)
    , m_should_reconnect(false)
    , m_current_host("localhost")
    , m_current_port(80)
    , m_current_path("/")
    , m_use_ssl(false)
{
}

WebSocketClient::~WebSocketClient()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool WebSocketClient::Initialize(const WebSocketConfig& config)
{
    if (m_initialized) {
        LOG_WARNING("WebSocketClient already initialized");
        return false;
    }

    LOG_INFO("Initializing WebSocketClient with libwebsockets...");

    m_config = config;

    // Parse URL
    if (!ParseURL(m_config.url)) {
        LOG_ERROR("WebSocketClient: Invalid URL: " + m_config.url);
        return false;
    }

    // Create libwebsockets context
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = websocket_protocols;
    info.options = 0;
    info.user = this;
    info.timeout_secs = m_config.timeout_seconds;

    m_context = lws_create_context(&info);
    if (!m_context) {
        LOG_ERROR("WebSocketClient: Failed to create libwebsockets context");
        return false;
    }

    LOG_INFO("WebSocketClient initialized successfully for " + m_config.url);
    m_initialized = true;
    return true;
}

void WebSocketClient::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    LOG_INFO("Shutting down WebSocketClient...");

    // Stop reconnect thread
    m_should_stop = true;
    m_should_reconnect = false;
    m_write_condition.notify_all();
    if (m_reconnect_thread.joinable()) {
        m_reconnect_thread.join();
    }

    // Disconnect if connected
    if (m_wsi) {
        lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
        m_wsi = nullptr;
    }

    // Clear message queue
    ClearQueuedMessages();

    // Destroy libwebsockets context
    if (m_context) {
        lws_context_destroy(m_context);
        m_context = nullptr;
    }

    m_connection_state = ConnectionState::Disconnected;
    m_initialized = false;
    LOG_INFO("WebSocketClient shutdown complete");
}

bool WebSocketClient::Connect()
{
    if (!m_initialized) {
        LOG_ERROR("WebSocketClient: Cannot connect - not initialized");
        return false;
    }

    if (m_connection_state == ConnectionState::Connected ||
        m_connection_state == ConnectionState::Connecting) {
        LOG_WARNING("WebSocketClient: Already connected or connecting");
        return false;
    }

    LOG_INFO("Connecting to WebSocket server: " + m_config.url);

    SetConnectionState(ConnectionState::Connecting);
    m_reconnect_attempts = 0;

    // Perform actual connection
    if (PerformConnection()) {
        return true;
    }

    // If auto-reconnect is enabled, start reconnect thread
    if (m_config.auto_reconnect) {
        m_should_reconnect = true;
        m_reconnect_thread = std::thread(&WebSocketClient::ReconnectThread, this);
    }

    return true;
}

void WebSocketClient::Disconnect()
{
    if (!m_initialized || m_connection_state == ConnectionState::Disconnected) {
        return;
    }

    LOG_INFO("Disconnecting from WebSocket server");

    m_should_reconnect = false;

    if (m_wsi) {
        lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
        m_wsi = nullptr;
    }

    SetConnectionState(ConnectionState::Disconnected);
}

bool WebSocketClient::SendMessage(const std::string& message)
{
    return SendMessage(message, 0);
}

bool WebSocketClient::SendMessage(const std::string& message, int priority)
{
    if (!m_initialized) {
        LOG_ERROR("WebSocketClient: Cannot send message - not initialized");
        return false;
    }

    if (m_connection_state != ConnectionState::Connected) {
        LOG_WARNING("WebSocketClient: Cannot send message - not connected");
        return false;
    }

    std::lock_guard<std::mutex> lock(m_queue_mutex);

    // Check queue size limit
    if (m_outgoing_queue.size() >= m_config.max_queue_size) {
        LOG_WARNING("WebSocketClient: Message queue full, dropping message");
        return false;
    }

    QueuedMessage queued_msg;
    queued_msg.data = message;
    queued_msg.priority = priority;
    m_outgoing_queue.push(queued_msg);

    // Request callback for writable event
    if (m_wsi) {
        lws_callback_on_writable(m_wsi);
    }

    LOG_DEBUG("WebSocket message queued (priority: " + std::to_string(priority) + ")");
    return true;
}

void WebSocketClient::SetMessageCallback(MessageCallback callback)
{
    std::lock_guard<std::mutex> lock(m_callback_mutex);
    m_message_callback = std::move(callback);
}

void WebSocketClient::SetConnectionCallback(ConnectionCallback callback)
{
    std::lock_guard<std::mutex> lock(m_callback_mutex);
    m_connection_callback = std::move(callback);
}

void WebSocketClient::SetErrorCallback(ErrorCallback callback)
{
    std::lock_guard<std::mutex> lock(m_callback_mutex);
    m_error_callback = std::move(callback);
}

void WebSocketClient::Update(float delta_time)
{
    if (!m_initialized) {
        return;
    }

    // Service libwebsockets with timeout
    if (m_context) {
        int timeout_ms = static_cast<int>(delta_time * 1000);
        lws_service(m_context, timeout_ms);
    }

    (void)delta_time;
}

ConnectionState WebSocketClient::GetConnectionState() const
{
    return m_connection_state.load();
}

bool WebSocketClient::IsInitialized() const
{
    return m_initialized.load();
}

size_t WebSocketClient::GetQueuedMessageCount() const
{
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    return m_outgoing_queue.size();
}

void WebSocketClient::ClearQueuedMessages()
{
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    while (!m_outgoing_queue.empty()) {
        m_outgoing_queue.pop();
    }
}

void WebSocketClient::ReconnectThread()
{
    LOG_INFO("WebSocket reconnect thread started");

    while (!m_should_stop && m_should_reconnect) {
        if (m_reconnect_attempts >= m_config.max_reconnect_attempts) {
            LOG_WARNING("WebSocket: Max reconnect attempts reached");
            HandleError("Max reconnect attempts reached");
            m_should_reconnect = false;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.reconnect_delay_ms));

        if (m_should_stop) {
            break;
        }

        m_reconnect_attempts++;
        LOG_INFO("WebSocket: Reconnect attempt " + std::to_string(m_reconnect_attempts));

        SetConnectionState(ConnectionState::Reconnecting);

        if (PerformConnection()) {
            m_reconnect_attempts = 0;
            break;
        }
    }

    LOG_INFO("WebSocket reconnect thread stopped");
}

bool WebSocketClient::PerformConnection()
{
    struct lws_client_connect_info i;
    memset(&i, 0, sizeof(i));

    i.context = m_context;
    i.address = m_current_host.c_str();
    i.path = m_current_path.c_str();
    i.host = m_current_host.c_str();
    i.origin = m_current_host.c_str();
    i.port = m_current_port;
    i.ssl_connection = m_use_ssl ? 1 : 0;
    i.protocol = websocket_protocols[0].name;

    m_wsi = lws_client_connect_via_info(&i);
    if (!m_wsi) {
        HandleError("Failed to create WebSocket connection to " + m_current_host);
        SetConnectionState(ConnectionState::Error);
        return false;
    }

    // Connection will be established asynchronously via callback
    return true;
}

void WebSocketClient::ProcessOutgoingMessages()
{
    if (!m_wsi || m_connection_state != ConnectionState::Connected) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_queue_mutex);

    while (!m_outgoing_queue.empty()) {
        QueuedMessage msg = m_outgoing_queue.top();
        m_outgoing_queue.pop();

        // Send message using libwebsockets
        unsigned char* buf = new unsigned char[LWS_PRE + msg.data.length()];
        memcpy(buf + LWS_PRE, msg.data.c_str(), msg.data.length());

        int result = lws_write(m_wsi, buf + LWS_PRE, msg.data.length(), LWS_WRITE_TEXT);
        delete[] buf;

        if (result < 0) {
            LOG_ERROR("WebSocket: Failed to send message");
            HandleError("Failed to send message");
            break;
        }

        LOG_DEBUG("WebSocket message sent successfully");
    }
}

void WebSocketClient::SetConnectionState(ConnectionState state)
{
    m_connection_state = state;

    std::lock_guard<std::mutex> lock(m_callback_mutex);
    if (m_connection_callback) {
        m_connection_callback(state);
    }
}

void WebSocketClient::HandleError(const std::string& error)
{
    LOG_ERROR("WebSocket error: " + error);

    std::lock_guard<std::mutex> lock(m_callback_mutex);
    if (m_error_callback) {
        m_error_callback(error);
    }
}

bool WebSocketClient::ParseURL(const std::string& url)
{
    if (url.empty()) {
        return false;
    }

    std::string parsed_url = url;

    // Extract protocol
    size_t protocol_end = parsed_url.find("://");
    if (protocol_end == std::string::npos) {
        m_use_ssl = false;
    } else {
        std::string protocol = parsed_url.substr(0, protocol_end);
        m_use_ssl = (protocol == "wss");
        parsed_url = parsed_url.substr(protocol_end + 3);
    }

    // Extract host and port
    size_t port_start = parsed_url.find(':');
    size_t path_start = parsed_url.find('/');

    if (port_start != std::string::npos) {
        // Port specified
        m_current_host = parsed_url.substr(0, port_start);
        
        size_t port_end = parsed_url.find('/', port_start);
        if (port_end != std::string::npos) {
            std::string port_str = parsed_url.substr(port_start + 1, port_end - port_start - 1);
            m_current_port = std::stoi(port_str);
            m_current_path = parsed_url.substr(port_end);
        } else {
            std::string port_str = parsed_url.substr(port_start + 1);
            m_current_port = std::stoi(port_str);
            m_current_path = "/";
        }
    } else if (path_start != std::string::npos) {
        // No port, path specified
        m_current_host = parsed_url.substr(0, path_start);
        m_current_port = m_use_ssl ? 443 : 80;
        m_current_path = parsed_url.substr(path_start);
    } else {
        // No port, no path
        m_current_host = parsed_url;
        m_current_port = m_use_ssl ? 443 : 80;
        m_current_path = "/";
    }

    // Default to localhost if empty
    if (m_current_host.empty()) {
        m_current_host = "localhost";
    }

    return true;
}

// Callback handlers
void WebSocketClient::OnConnectionEstablished()
{
    LOG_INFO("WebSocket connection established");
    SetConnectionState(ConnectionState::Connected);
    m_reconnect_attempts = 0;
}

void WebSocketClient::OnMessageReceived(const std::string& message)
{
    LOG_DEBUG("WebSocket message received: " + message);

    std::lock_guard<std::mutex> lock(m_callback_mutex);
    if (m_message_callback) {
        m_message_callback(message);
    }
}

void WebSocketClient::OnConnectionClosed()
{
    LOG_INFO("WebSocket connection closed");
    m_wsi = nullptr;
    
    if (m_connection_state == ConnectionState::Connected) {
        SetConnectionState(ConnectionState::Disconnected);
        
        // Start reconnect if enabled
        if (m_config.auto_reconnect && !m_should_stop) {
            m_should_reconnect = true;
            if (!m_reconnect_thread.joinable()) {
                m_reconnect_thread = std::thread(&WebSocketClient::ReconnectThread, this);
            }
        }
    }
}

void WebSocketClient::OnConnectionError(const std::string& error)
{
    LOG_ERROR("WebSocket connection error: " + error);
    m_wsi = nullptr;
    HandleError(error);
    SetConnectionState(ConnectionState::Error);
}

void WebSocketClient::OnWritable()
{
    ProcessOutgoingMessages();
}

} // namespace Networking
} // namespace Poko
