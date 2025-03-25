#include <iostream>
#include <poll.h>
#include <thread>

#include "entropy_calculator.h"
#include "event_detector.h"
#include "file_extension_checker.h"
#include "websocket_server.h"

using namespace std;

const char* WEBSOCKET_URI = "ws://172.16.182.135:9002";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: must specify a directory" << endl;
        return -1;
    }

    // Give system/network a moment to stabilize
    this_thread::sleep_for(std::chrono::milliseconds(500));

    thread ws_client_thread(connect_to_websocket_host, WEBSOCKET_URI);

    Logger* logger = Logger::getInstance();
    logger->log("Starting detector...");

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

    return 0;
}
