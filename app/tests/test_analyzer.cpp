#include <gtest/gtest.h>
#include "analyzer.h"
#include "event.h"
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// Test fixture for Analyzer
class AnalyzerTest : public ::testing::Test {
protected:
    Analyzer analyzer;
};

// Test suspicious file extension should immediately trigger removal
TEST_F(AnalyzerTest, SuspiciousFileExtensionTriggersRemoval) {
    time_t now = time(nullptr);

    // ".crypt" is in the blacklist.
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/ransomware.crypt",
                "ransomware.crypt",
                now,
                1001);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("flagging for removal"), string::npos);
}

// Test compressed file triggers monobit suspicion (entropy-based)
TEST_F(AnalyzerTest, CompressedFileMonobitTriggersSuspicion) {
    time_t now = time(nullptr);

    // Assuming encrypted_compressed.zip is suspicious.
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/encrypted_compressed.zip",
                "encrypted_compressed.zip",
                now,
                1002);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("flagging for removal"), string::npos);
}

// Test low-entropy file should not be flagged
TEST_F(AnalyzerTest, LowEntropyFileIsNotSuspicious) {
    time_t now = time(nullptr);

    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/plain_text.txt",
                "plain_text.txt",
                now,
                1004);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("is not suspicious"), string::npos);
}

// Test whitelisted high-entropy files (like ZIPs) are not flagged if normal
TEST_F(AnalyzerTest, WhitelistedHighEntropyExtensionPassesMonobitTest) {
    time_t now = time(nullptr);

    // This ZIP file should be in the whitelist and not suspicious.
    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/plain_compressed.zip",
                "plain_compressed.zip",
                now,
                1003);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("is not suspicious"), string::npos);
}

// Test small files are skipped and not flagged
TEST_F(AnalyzerTest, SmallFileIsIgnored) {
    time_t now = time(nullptr);

    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/small_file.txt",
                "small_file.txt",
                now,
                1010);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("is not suspicious"), string::npos);
}

// Test blacklisted extensions should trigger immediate suspicion
TEST_F(AnalyzerTest, BlacklistedExtensionImmediateRemoval) {
    time_t now = time(nullptr);

    Event event("FAN_CLOSE_WRITE",
                "tests/test_files/fake_ransom.locked",
                "fake_ransom.locked",
                now,
                1011);

    testing::internal::CaptureStdout();
    analyzer.analyze(event);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("flagging for removal"), string::npos);
}
