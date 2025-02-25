
#include <iostream>
#include <poll.h>
#include "entropy_calculator.h"
#include "event_detector.h"
#include "file_extension_checker.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: must specify a filepath" << endl;
        return -1;
    }
    EntropyCalculator ec;
    cout << "File entropy: " << ec.get_shannon_entropy(argv[1]) << endl;
    bool is_random = ec.monobit_test(argv[1]);
    cout << "Monobit result: " << is_random << endl;

    FileExtensionChecker fec;
    bool is_compressed = fec.is_compressed(argv[1]);
    cout << "Is file compressed?: " << is_compressed << endl;
    bool is_suspicious = fec.is_suspicious(argv[1]);
    cout << "Is file suspicious?: " << is_suspicious << endl;
    bool is_image = fec.is_image(argv[1]);
    cout << "Is file an image?: " << is_image << endl;

    EventDetector detector;
    detector.add_watch("/home");

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