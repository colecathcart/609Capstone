#include <gtest/gtest.h>
#include "logger.h"
#include <fstream>
#include <cstdio>

using namespace std;

// // Test fixture for Logger
class LoggerTest : public ::testing::Test {
protected:
    Logger& logger = Logger::getInstance();

    void SetUp() override {
        logger.set_destination(1); // Log to file
        ofstream ofs("log.txt", ios::trunc); // Clear log file before test
        ofs.close();
    }

    void TearDown() override {
        // Nothing for now
    }
};

// Test same instance of the singleton Logger is returned
TEST_F(LoggerTest, SingletonReturnsSameInstance) {
    Logger& second = Logger::getInstance();
    EXPECT_EQ(&logger, &second); // Compare addresses since we're using references
}

// Test logging to file
TEST_F(LoggerTest, LogsToFileCorrectly) {
    string test_msg = "Test log message";
    logger.log(test_msg);

    ifstream file("log.txt");
    string line;
    bool found = false;
    while (getline(file, line)) {
        if (line == test_msg) {
            found = true;
            break;
        }
    }
    file.close();
    EXPECT_TRUE(found);
}
