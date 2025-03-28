#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <string>
using namespace std;

// Starts the WebSocket++ server (host mode)
void start_websocket_server();

// Connects to a WebSocket++ host server (client mode)
void connect_to_websocket_host(const string& uri);

// Called by Analyzer to send stat updates ("SUSPICIOUS" or "KILLED")
void send_stat_update(const std::string& type);

// Insert or update a device's status (e.g., "Online", "Offline", "Threat Detected")
void set_device_status(const std::string& name, const std::string& status);

// Returns a combined JSON payload (stats, device statuses, blacklisted hashes)
string get_combined_payload();

// Get the local device hostname
string get_device_name();

// Increment a numeric field in system_stats (e.g., "suspicious_detected")
void increment_stat(const std::string& field);

// Add a hash to the blacklist_hashes table and broadcast
void add_blacklisted_hash(const std::string& hash);

#endif // WEBSOCKET_SERVER_H
