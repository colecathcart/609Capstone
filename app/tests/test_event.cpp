#include <gtest/gtest.h>
#include "event.h"
#include <string>
#include <ctime>

using namespace std;

// Test fixture for Event
class EventTest : public ::testing::Test {
protected:
    time_t now = time(nullptr);
    Event event = Event("FAN_CLOSE_WRITE", "/home/user/file.txt", "file.txt", now, 1234);
};

// Test constructor + getters
TEST_F(EventTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(event.get_event_type(), "FAN_CLOSE_WRITE");
    EXPECT_EQ(event.get_filepath(), "/home/user/file.txt");
    EXPECT_EQ(event.get_filename(), "file.txt");
    EXPECT_EQ(event.get_time(), now);
    EXPECT_EQ(event.get_pid(), 1234);
}

// Test default constructor
TEST_F(EventTest, DefaultConstructorDoesNotCrash) {
    Event default_event;
    EXPECT_NO_THROW(default_event.get_event_type());
}

// Test print format (partial match)
TEST_F(EventTest, PrintReturnsFormattedString) {
    std::string output = event.print();
    EXPECT_NE(output.find("FAN_CLOSE_WRITE"), string::npos);
    EXPECT_NE(output.find("file.txt"), string::npos);
    EXPECT_NE(output.find(to_string(now)), string::npos);
    EXPECT_NE(output.find("1234"), string::npos);
}
