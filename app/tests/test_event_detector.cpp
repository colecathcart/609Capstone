#include <gtest/gtest.h>
#include "event_detector.h"
#include <filesystem>

using namespace std;

// Test fixture for EventDetector
class EventDetectorTest : public ::testing::Test {
protected:
    EventDetector detector;
};

// Test that get_fanotify_fd returns a valid descriptor
TEST_F(EventDetectorTest, FanotifyFdIsValid) {
    int fd = detector.get_fanotify_fd();
    EXPECT_GE(fd, 0); // Should be non-negative
}

// Test that add_watch executes without crashing for an existing directory
TEST_F(EventDetectorTest, AddWatchDoesNotThrow) {
    string test_dir = "./test_watch_dir";
    filesystem::create_directory(test_dir);

    EXPECT_NO_THROW({
        detector.add_watch(test_dir);
    });

    filesystem::remove_all(test_dir);
}
