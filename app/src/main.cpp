#include <iostream>
#include <poll.h>
#include <thread>
#include <csignal>

#include "entropy_calculator.h"
#include "event_detector.h"
#include "file_extension_checker.h"
#include "websocket_server.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <atomic>
#include <thread_pool.h>

using namespace std;

#define WEBSOCKET_URI "ws://localhost:9002"
atomic<bool> running(true);

void signal_handler(int signal) {
    running.store(false, memory_order_relaxed);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: must specify a directory" << endl;
        return -1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    thread ws_client_thread(connect_to_websocket_host, WEBSOCKET_URI);

    Logger& logger = Logger::getInstance();
    if (argc == 3) {
        logger.set_destination(stoi(argv[2]));
    }
    logger.log("Starting detector...");

    unsigned int num_threads = thread::hardware_concurrency() - 1;
    if (num_threads < 1) {num_threads = 1;};
    ThreadPool pool(num_threads);

    EventDetector detector;
    detector.add_watch(argv[1]);

    struct pollfd pfd;
    pfd.fd = detector.get_fanotify_fd();
    pfd.events = POLLIN;

    while (running) {
        int ret = poll(&pfd, 1, -1);
        if (ret > 0 && pfd.revents & POLLIN) {
            detector.process_events(pool, running);
        }
    }

    logger.log("\nstopping detector.");
    if(ws_client_thread.joinable()) {
        get_websocket_client().stop();
        ws_client_thread.join();
    }

    return 0;
}
