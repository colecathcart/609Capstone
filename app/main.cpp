
#include <iostream>
#include <poll.h>
#include "EventDetector.h"

using namespace std;

int main(int argc, char* argv[]) {
    EventDetector detector;
    detector.add_watch("/home/destin/Documents");

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