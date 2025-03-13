#include <gtest/gtest.h>
#include "analyzer.h"
#include "event.h"
#include <ctime>
#include <string>
#include <iostream>
using namespace std;

// Test fixture for Analyzer
class AnalyzerTest : public ::testing::Test {
protected:
    Analyzer analyzer;
};

// Test suspicious file extension triggers removal
TEST_F(AnalyzerTest, SuspiciousFileExtensionTriggersRemoval) {
    time_t now = time(nullptr);

    // Using ".crypt" to simulate a suspicious extension.
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/ransomware.crypt",
                "ransomware.crypt",
                "crypt",
                now,
                1001);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect output to include "flagging for removal".
    EXPECT_NE(output.find("flagging for removal"), string::npos);
}

// Test monobit result for compressed file triggers watch update
TEST_F(AnalyzerTest, CompressedFileMonobitTestPassesUpdatesWatch) {
    time_t now = time(nullptr);

    // "encrypted_compressed.zip" is assumed high-entropy (e.g., truly encrypted).
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/encrypted_compressed.zip",
                "encrypted_compressed.zip",
                "zip",
                now,
                1002);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect "is suspicious, updating watch" in the output.
    EXPECT_NE(output.find("is suspicious, updating watch"), string::npos);
}

// Test low entropy file is not suspicious
TEST_F(AnalyzerTest, LowEntropyFileIsNotSuspicious) {
    time_t now = time(nullptr);

    // "plain_text.txt" should have lower entropy.
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/plain_text.txt",
                "plain_text.txt",
                "txt",
                now,
                1004);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect "is not suspicious" in the output.
    EXPECT_NE(output.find("is not suspicious"), string::npos);
}

// Test repeated suspicion triggers removal
TEST_F(AnalyzerTest, UpdateWatchTriggersRemovalOnRepeatedSuspicion) {
    // Simulate the same PID being suspicious twice within an hour.
    time_t now = time(nullptr);

    Event event1("FAN_CLOSE_WRITE",
                 "tests/test_files/encrypted_image.jpg",
                 "encrypted_image.jpg",
                 "jpg",
                 now,
                 1005);

    // 100 seconds later—still within 3600s threshold.
    Event event2("FAN_CLOSE_WRITE",
                 "tests/test_files/encrypted_image.jpg",
                 "encrypted_image.jpg",
                 "jpg",
                 now + 100,
                 1005);

    // First call: should update watch (suspicious).
    analyzer.analyze(event1);

    // Second call: triggers removal because it’s the same PID again soon.
    testing::internal::CaptureStdout();
    analyzer.analyze(event2);
    string output2 = testing::internal::GetCapturedStdout();

    // Expect "flagging for removal" in the second output.
    EXPECT_NE(output2.find("flagging for removal"), string::npos);
}
