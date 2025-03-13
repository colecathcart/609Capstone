#include <gtest/gtest.h>
#include "analyzer.h"
#include "event.h"
#include <ctime>
#include <string>
#include <iostream>
using namespace std;

class AnalyzerTest : public ::testing::Test {
protected:
    Analyzer analyzer;
};

TEST_F(AnalyzerTest, SuspiciousFileExtensionTriggersRemoval) {
    // Using a ".crypt" extension to simulate a suspicious file.
    // If you want this file to actually exist, place a dummy file in tests/test_files.
    time_t now = time(nullptr);
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/ransomware.crypt",
                "ransomware.crypt",
                "crypt",
                now,
                1001);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect output to include a "flagging for removal" message.
    EXPECT_NE(output.find("flagging for removal"), string::npos);
}

TEST_F(AnalyzerTest, CompressedFileMonobitTestPassesUpdatesWatch) {
    // We assume "encrypted_compressed.zip" is a high-entropy compressed file.
    // The Analyzer may apply the monobit test if your FileExtensionChecker recognizes ".zip" as compressed.
    time_t now = time(nullptr);
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/encrypted_compressed.zip",
                "encrypted_compressed.zip",
                "zip",
                now,
                1002);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect the output to mention the process is suspicious and the watch is updated.
    EXPECT_NE(output.find("is suspicious, updating watch"), string::npos);
}

TEST_F(AnalyzerTest, LowEntropyFileIsNotSuspicious) {
    // "plain_text.txt" should have lower entropy.
    time_t now = time(nullptr);
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/plain_text.txt",
                "plain_text.txt",
                "txt",
                now,
                1004);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    // Expect the output to indicate that the process is not suspicious.
    EXPECT_NE(output.find("is not suspicious"), string::npos);
}

TEST_F(AnalyzerTest, UpdateWatchTriggersRemovalOnRepeatedSuspicion) {
    // We simulate the same process (PID 1005) being suspicious twice within an hour.
    // In this case, we're using "encrypted_image.jpg" to trigger suspicion (e.g., via monobit or entropy tests).
    time_t now = time(nullptr);

    Event event1("FAN_CLOSE_WRITE",
                 "tests/test_files/encrypted_image.jpg",
                 "encrypted_image.jpg",
                 "jpg",
                 now,
                 1005);

    Event event2("FAN_CLOSE_WRITE",
                 "tests/test_files/encrypted_image.jpg",
                 "encrypted_image.jpg",
                 "jpg",
                 now + 100, // 100 seconds later
                 1005);

    // First call: should update watch (suspicious, but not enough to remove yet).
    testing::internal::CaptureStdout();
    analyzer.analyze(event1);
    string output1 = testing::internal::GetCapturedStdout();

    // Second call: same process, still suspicious within 3600 seconds => triggers removal.
    testing::internal::CaptureStdout();
    analyzer.analyze(event2);
    string output2 = testing::internal::GetCapturedStdout();

    // Expect the second output to mention "flagging for removal."
    EXPECT_NE(output2.find("flagging for removal"), string::npos);
}

