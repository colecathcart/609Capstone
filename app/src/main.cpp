#include <iostream>
#include <poll.h>
#include <thread> 

#include "entropy_calculator.h"
#include "event_detector.h"
#include "file_extension_checker.h"
#include "websocket_server.h"  

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: must specify a directory" << endl;
        return -1;
    }

    Logger* logger = Logger::getInstance();
    logger->log("Starting detector...");

    // Start WebSocket++ server in background
    thread ws_thread(start_websocket_server);

    EventDetector detector;
    detector.add_watch(argv[1]);

    struct pollfd pfd;
    pfd.fd = detector.get_fanotify_fd();
    pfd.events = POLLIN;

    while (true) {
        int ret = poll(&pfd, 1, -1);
        if (ret > 0 && pfd.revents & POLLIN) {
            detector.process_events();
        }
    }

    // Cleanup 
    ws_thread.join();
    return 0;
}
