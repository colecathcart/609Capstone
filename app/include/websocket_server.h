#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <string>
using namespace std;

// Starts the WebSocket++ server (host mode)
void start_websocket_server();

// Connects to a WebSocket++ host server (client mode)
void connect_to_websocket_host(const string& uri);

// Called by Analyzer to send stat updates ("SUSPICIOUS" or "KILLED")
void send_stat_update(const string& type);

// Updates or inserts a device's status (Online, Offline, Threat Detected, etc.)
void set_device_status(const string& name, const string& status);

// Returns the combined JSON payload of system stats + device statuses
string get_combined_payload();

// Returns the current device hostname
string get_device_name();

#endif // WEBSOCKET_SERVER_H
