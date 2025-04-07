#include <gtest/gtest.h>
#include "logger.h"
#include <fstream>
#include <cstdio>

class LoggerTest : public ::testing::Test {
protected:
    Logger* logger;

    void SetUp() override {
        logger = Logger::getInstance();
        logger->set_destination(1); // File only
        ofstream ofs("log.txt", ios::trunc); // Clear log file before test
        ofs.close();
    }

    void TearDown() override {
    }
};

TEST_F(LoggerTest, SingletonReturnsSameInstance) {
    Logger* second = Logger::getInstance();
    EXPECT_EQ(logger, second);
}

TEST_F(LoggerTest, LogsToFileCorrectly) {
    string test_msg = "Test log message";
    logger->log(test_msg);

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
