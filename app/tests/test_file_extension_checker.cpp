#include <gtest/gtest.h>
#include "file_extension_checker.h"

// Create a test fixture for FileExtensionChecker
class FileExtensionCheckerTest : public ::testing::Test {
protected:
    FileExtensionChecker checker;  // Instance to be used in all tests
};

// Test for Compressed File Extension
TEST_F(FileExtensionCheckerTest, CompressedFileTest) {
    EXPECT_TRUE(checker.is_compressed("archive.zip"));
    EXPECT_TRUE(checker.is_compressed("backup.rar"));
    EXPECT_FALSE(checker.is_compressed("document.txt"));
}

// Test for Suspicious File Extension
TEST_F(FileExtensionCheckerTest, SuspiciousFileTest) {
    EXPECT_TRUE(checker.is_suspicious("malware.crypt"));
    EXPECT_TRUE(checker.is_suspicious("ransomware.lock"));
    EXPECT_FALSE(checker.is_suspicious("safe_file.docx"));
}

// Test for Image File Extension
TEST_F(FileExtensionCheckerTest, ImageFileTest) {
    EXPECT_TRUE(checker.is_image("photo.jpg"));
    EXPECT_TRUE(checker.is_image("picture.png"));
    EXPECT_FALSE(checker.is_image("video.mp4"));
}
