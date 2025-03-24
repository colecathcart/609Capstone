#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <string>
using namespace std;

// Starts the WebSocket++ server 
void start_websocket_server();

// Called when suspicious behavior is detected
void increment_suspicious();

// Called when a process is removed
void increment_killed();

// Updates or inserts a device's status (Online/Offline)
void set_device_status(const std::string& name, const std::string& status);

// Returns combined JSON payload of stats and device statuses
string get_combined_payload();

#endif
