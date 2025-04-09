#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <string>
using namespace std;
/**
 * @brief Starts the WebSocket++ server (host mode)
 */
void start_websocket_server();

/**
 * @brief Connects to a WebSocket++ host server (client mode)
 * @param uri The uri to attempt connection with
 */
void connect_to_websocket_host(const string& uri);

/**
 * @brief Called by Analyzer to send stat updates ("SUSPICIOUS" or "KILLED")
 * @param type The type of update to send ("SUSPICIOUS" or "KILLED")
 */
void send_stat_update(const std::string& type);

/**
 * @brief Insert or update a device's status
 * @param name The name of the device to update
 * @param status The status update (e.g., "Online", "Offline", "Threat Detected")
 */
void set_device_status(const std::string& name, const std::string& status);

/**
 * @brief Returns a combined JSON payload (stats, device statuses, blacklisted hashes)
 */
string get_combined_payload();

/**
 * @brief Returns the device name
 */
string get_device_name();


/**
 * @brief Increment a numeric field in system_stats
 * @param field The field to increment (e.g., "suspicious_detected")
 */
void increment_stat(const std::string& field);

/**
 * @brief Add a hash to the blacklist_hashes table and broadcast
 * @param hash The hash to broadcast
 */
void add_blacklisted_hash(const std::string& hash);

#endif
