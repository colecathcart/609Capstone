#include <gtest/gtest.h>
#include "thread_pool.h"
#include "event.h"
#include <fstream>
#include <string>
#include <cstdio>
#include <thread>
#include <chrono>

using namespace std;

// Helper to wait for log file to contain the expected message
bool wait_for_log(const string& expected, int timeout_ms = 1000) {
    int waited = 0;
    while (waited < timeout_ms) {
        ifstream log("log.txt");
        string line;
        while (getline(log, line)) {
            if (line.find(expected) != string::npos) {
                return true;
            }
        }
        log.close();
        this_thread::sleep_for(chrono::milliseconds(100));
        waited += 100;
    }
    return false;
}

// Test that analyzer is called and event is logged
TEST(ThreadPoolTest, AnalyzeCalledThroughThreadPool) {
    // Clear previous log
    ofstream ofs("log.txt", ios::trunc);
    ofs.close();

    // Create a dummy event
    Event e("FAN_CLOSE_WRITE", "/tmp/testfile.txt", "testfile.txt", time(nullptr), 1234);

    // Create a thread pool with 2 threads
    ThreadPool pool(2);

    // Add the event to the pool
    pool.add_event(e);

    // Verify log was updated by Analyzer
    ASSERT_TRUE(wait_for_log("FAN_CLOSE_WRITE"));
}
