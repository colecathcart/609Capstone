
#include <iostream>
#include "EventDetector.h"

using namespace std;

int main(int argc, char* argv[]) {
    EventDetector detector;
    detector.add_watch("/tmp");

    while (true) {
        detector.process_events();
    }

    return 0;
}