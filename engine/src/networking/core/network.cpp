/**
 * @file network.cpp
 * @brief Network system implementation for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "networking/network.h"
#include "core/logging/logger.h"
#include <cstring>
#include <map>
#include <mutex>

namespace poko {
namespace networking {

// Network connection implementation
struct NetworkConnectionImpl {
    uint64_t id;
    std::string address;
    int port;
    bool connected;
    NetworkEventCallback event_callback;
    void* callback_user_data;
    NetworkConnectionImpl* next;
};

// Network manager
struct NetworkManagerImpl {
    std::map<uint64_t, NetworkConnectionImpl*> connections;
    std::mutex mutex;
    uint64_t next_connection_id;
};

// Convert to internal implementation
static NetworkConnectionImpl* to_connection_impl(NetworkConnection* connection) {
    return reinterpret_cast<NetworkConnectionImpl*>(connection);
}

static const NetworkConnectionImpl* to_connection_impl(const NetworkConnection* connection) {
    return reinterpret_cast<const NetworkConnectionImpl*>(connection);
}

static NetworkConnection* from_connection_impl(NetworkConnectionImpl* impl) {
    return reinterpret_cast<NetworkConnection*>(impl);
}

} // namespace networking
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::networking;

NetworkConnection* network_connect(const char* address, int port) {
    if (!address || port <= 0) return nullptr;
    
    static NetworkManagerImpl* manager = nullptr;
    if (!manager) {
        manager = new NetworkManagerImpl();
        manager->next_connection_id = 1;
    }
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    NetworkConnectionImpl* connection = new NetworkConnectionImpl();
    connection->id = manager->next_connection_id++;
    connection->address = address;
    connection->port = port;
    connection->connected = false;
    connection->event_callback = nullptr;
    connection->callback_user_data = nullptr;
    connection->next = nullptr;
    
    // In a real implementation, this would establish actual network connection
    // For now, this is a placeholder that simulates connection
    connection->connected = true;
    
    // Add to manager
    manager->connections[connection->id] = connection;
    
    POKO_LOG_INFO("Network: Connected to " + std::string(address) + ":" + std::to_string(port));
    
    if (connection->event_callback) {
        connection->event_callback(NETWORK_EVENT_CONNECTED, nullptr, 0, connection->callback_user_data);
    }
    
    return from_connection_impl(connection);
}

bool network_disconnect(NetworkConnection* connection) {
    if (!connection) return false;
    
    static NetworkManagerImpl* manager = nullptr;
    if (!manager) return false;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    NetworkConnectionImpl* impl = to_connection_impl(connection);
    
    if (impl->event_callback) {
        impl->event_callback(NETWORK_EVENT_DISCONNECTED, nullptr, 0, impl->callback_user_data);
    }
    
    impl->connected = false;
    
    // Remove from manager
    manager->connections.erase(impl->id);
    
    delete impl;
    
    POKO_LOG_INFO("Network: Disconnected from " + impl->address + ":" + std::to_string(impl->port));
    return true;
}

bool network_send(NetworkConnection* connection, const char* data, size_t size) {
    if (!connection || !data || size == 0) return false;
    
    NetworkConnectionImpl* impl = to_connection_impl(connection);
    
    if (!impl->connected) {
        POKO_LOG_WARN("Network: Attempted to send on disconnected connection");
        return false;
    }
    
    // In a real implementation, this would send actual data over the network
    POKO_LOG_DEBUG("Network: Sent " + std::to_string(size) + " bytes to " + impl->address);
    
    return true;
}

int network_receive(NetworkConnection* connection, char* buffer, size_t size) {
    if (!connection || !buffer || size == 0) return -1;
    
    NetworkConnectionImpl* impl = to_connection_impl(connection);
    
    if (!impl->connected) {
        POKO_LOG_WARN("Network: Attempted to receive on disconnected connection");
        return -1;
    }
    
    // In a real implementation, this would receive actual data from the network
    // For now, return 0 to indicate no data available
    return 0;
}

void network_set_event_callback(NetworkConnection* connection, NetworkEventCallback callback, void* user_data) {
    if (!connection) return;
    
    NetworkConnectionImpl* impl = to_connection_impl(connection);
    impl->event_callback = callback;
    impl->callback_user_data = user_data;
    
    POKO_LOG_DEBUG("Network: Set event callback for connection to " + impl->address);
}

bool network_is_connected(const NetworkConnection* connection) {
    if (!connection) return false;
    
    const NetworkConnectionImpl* impl = to_connection_impl(connection);
    return impl->connected;
}

const char* network_get_address(const NetworkConnection* connection) {
    if (!connection) return "";
    
    const NetworkConnectionImpl* impl = to_connection_impl(connection);
    return impl->address.c_str();
}

int network_get_port(const NetworkConnection* connection) {
    if (!connection) return 0;
    
    const NetworkConnectionImpl* impl = to_connection_impl(connection);
    return impl->port;
}

} // extern "C"