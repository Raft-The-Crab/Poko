/**
 * @file network.h
 * @brief Network system interface for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_NETWORK_H
#define POKO_NETWORK_H

#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

// Network connection handle (opaque)
typedef struct NetworkConnection NetworkConnection;

// Network event types
typedef enum {
    NETWORK_EVENT_CONNECTED = 0,
    NETWORK_EVENT_DISCONNECTED = 1,
    NETWORK_EVENT_DATA_RECEIVED = 2,
    NETWORK_EVENT_ERROR = 3
} NetworkEventType;

// Network event callback
typedef void (*NetworkEventCallback)(NetworkEventType event, const void* data, size_t size, void* user_data);

/**
 * Create a network connection
 * @param address Server address (IP or hostname)
 * @param port Server port
 * @return Network connection handle, or NULL on failure
 */
NetworkConnection* network_connect(const char* address, int port);

/**
 * Disconnect and destroy a network connection
 * @param connection Network connection handle
 * @return true on success, false on failure
 */
bool network_disconnect(NetworkConnection* connection);

/**
 * Send data over network
 * @param connection Network connection handle
 * @param data Data to send
 * @param size Data size
 * @return true on success, false on failure
 */
bool network_send(NetworkConnection* connection, const char* data, size_t size);

/**
 * Receive data from network
 * @param connection Network connection handle
 * @param buffer Buffer to receive data
 * @param size Buffer size
 * @return Number of bytes received, or -1 on error
 */
int network_receive(NetworkConnection* connection, char* buffer, size_t size);

/**
 * Set event callback
 * @param connection Network connection handle
 * @param callback Event callback function
 * @param user_data User data to pass to callback
 */
void network_set_event_callback(NetworkConnection* connection, NetworkEventCallback callback, void* user_data);

/**
 * Check if connection is connected
 * @param connection Network connection handle
 * @return true if connected, false otherwise
 */
bool network_is_connected(const NetworkConnection* connection);

/**
 * Get connection address
 * @param connection Network connection handle
 * @return Connection address string
 */
const char* network_get_address(const NetworkConnection* connection);

/**
 * Get connection port
 * @param connection Network connection handle
 * @return Connection port
 */
int network_get_port(const NetworkConnection* connection);

#ifdef __cplusplus
}
#endif

#endif // POKO_NETWORK_H