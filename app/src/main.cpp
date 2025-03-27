
#include <iostream>
#include <poll.h>
#include "entropy_calculator.h"
#include "event_detector.h"
#include "file_extension_checker.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2) {
         cout << "Error: must specify a directory" << endl;
        return -1;
    }

    Logger* logger = Logger::getInstance();
    logger->log("Starting detector...");

    EntropyCalculator calc;
    cout << "starting test" << endl;
    //calc.calc_shannon_entropy("/home/vm/Desktop/vscode/609Capstone/encryption_zone/decoded.txt", 5);
    calc.calc_shannon_entropy("/home/vm/Desktop/vscode/609Capstone/encryption_zone/test6.zip.GNNCRY", 5);
    //calc.calc_shannon_entropy("/home/vm/Desktop/vscode/609Capstone/app/ASL_Alphabet_Classifier-main.zip", 2);
    cout << "Done tests" << endl;
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