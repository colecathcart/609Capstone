#include <gtest/gtest.h>
#include "event_detector.h"
#include "event.h" 
#include <ctime>

class EventDetectorTest : public ::testing::Test {
protected:
    EventDetector ed;
};

// Test hidden path detection
TEST_F(EventDetectorTest, HiddenPathDetection) {
    // Expect paths with hidden directories (starting with '.') to return true.
    EXPECT_TRUE(ed.is_hidden_path("/home/user/.hidden/file.txt"));
    EXPECT_TRUE(ed.is_hidden_path("./.config/settings.ini"));
    // Expect visible paths to return false.
    EXPECT_FALSE(ed.is_hidden_path("/home/user/visible/file.txt"));
}

// Test non concerning path detection
TEST_F(EventDetectorTest, NoConcernPathDetection) {
    // Paths in /tmp/ or /var/spool/ should be ignored.
    EXPECT_TRUE(ed.is_no_concern_path("/tmp/somefile.txt"));
    EXPECT_TRUE(ed.is_no_concern_path("/var/spool/print"));
    // Paths outside these directories should not be flagged.
    EXPECT_FALSE(ed.is_no_concern_path("/home/user/somefile.txt"));
}

// Test returning the fanotify file descriptor
TEST_F(EventDetectorTest, ValidFanotifyFd) {
    int fd = ed.get_fanotify_fd();
    // A valid file descriptor should be non-negative.
    EXPECT_GE(fd, 0);
}

// Test enqueuing an event
TEST_F(EventDetectorTest, EnqueueEventWorks) {
    // Create a dummy event.
    Event testEvent("TEST", "/tmp/test.txt", "test.txt", "txt", time(nullptr), 1234);
    // Ensure that enqueue_event does not throw any exceptions.
    ASSERT_NO_THROW(ed.enqueue_event(testEvent));
}
