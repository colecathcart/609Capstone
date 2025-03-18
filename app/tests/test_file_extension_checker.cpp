#include <gtest/gtest.h>
#include "file_extension_checker.h"

// Test fixture for FileExtensionChecker
class FileExtensionCheckerTest : public ::testing::Test {
protected:
    FileExtensionChecker checker;
};

// Test needs_monobit() function
TEST_F(FileExtensionCheckerTest, NeedsMonobitTest) {
    EXPECT_TRUE(checker.needs_monobit("tests/test_files/plain_image.jpg"));
    EXPECT_TRUE(checker.needs_monobit("tests/test_files/plain_compressed.zip"));
    EXPECT_FALSE(checker.needs_monobit("tests/test_files/plain_text.txt"));
}

// Test is_suspicious() function
TEST_F(FileExtensionCheckerTest, IsSuspiciousTest) {
    EXPECT_TRUE(checker.is_suspicious("malware.crypt"));
    EXPECT_TRUE(checker.is_suspicious("ransomware.lock"));
    EXPECT_TRUE(checker.is_suspicious("virus.enc"));
    EXPECT_FALSE(checker.is_suspicious("safe_file.docx"));
}

