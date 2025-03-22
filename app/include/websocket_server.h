#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

// Starts the WebSocket++ server 
void start_websocket_server();

// Called when suspicious behavior is detected
void increment_suspicious();

// Called when a process is removed
void increment_killed();

#endif
