#include <gtest/gtest.h>
#include "logger.h"
#include <fstream>
#include <cstdio>

class LoggerTest : public ::testing::Test {
protected:
    Logger& logger = Logger::getInstance();

    void SetUp() override {
        logger.set_destination(1); // Log to file
        std::ofstream ofs("log.txt", std::ios::trunc); // Clear log file before test
        ofs.close();
    }

    void TearDown() override {
        // Nothing for now
    }
};

TEST_F(LoggerTest, SingletonReturnsSameInstance) {
    Logger& second = Logger::getInstance();
    EXPECT_EQ(&logger, &second); // Compare addresses since we're using references
}

TEST_F(LoggerTest, LogsToFileCorrectly) {
    std::string test_msg = "Test log message";
    logger.log(test_msg);

    std::ifstream file("log.txt");
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line == test_msg) {
            found = true;
            break;
        }
    }
    file.close();
    EXPECT_TRUE(found);
}
