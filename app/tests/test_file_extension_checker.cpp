#include <gtest/gtest.h>
#include "file_extension_checker.h"

// Test fixture for FileExtensionChecker
class FileExtensionCheckerTest : public ::testing::Test {
protected:
    FileExtensionChecker checker;
};

// Test needs_monobit() based on MIME types
TEST_F(FileExtensionCheckerTest, NeedsMonobitTest) {
    // Files with MIME types like "image", "zip", etc. should be whitelisted
    EXPECT_TRUE(checker.needs_monobit("tests/test_files/plain_image.jpg"));         // "image"
    EXPECT_TRUE(checker.needs_monobit("tests/test_files/plain_compressed.zip"));    // "zip"
    EXPECT_FALSE(checker.needs_monobit("tests/test_files/plain_text.txt"));         // "text/plain" -> not in whitelist
}

// Test is_blacklist_extension() function
TEST_F(FileExtensionCheckerTest, IsBlacklistedExtensionTest) {
    EXPECT_TRUE(checker.is_blacklist_extension("malware.crypt"));
    EXPECT_TRUE(checker.is_blacklist_extension("ransomware.lock"));
    EXPECT_TRUE(checker.is_blacklist_extension("virus.enc"));
    EXPECT_FALSE(checker.is_blacklist_extension("safe_file.docx"));
    EXPECT_FALSE(checker.is_blacklist_extension("note"));  // No extension
}

