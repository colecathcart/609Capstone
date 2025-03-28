#include <thread>
#include <iostream>
#include "websocket_server.h"

using namespace std;

int main() {
    cout << "[MODE] Starting as HOST..." << endl;

    // Start the WebSocket server on a separate thread
    thread ws_thread(start_websocket_server);
    ws_thread.join();

    return 0;
}
