#include <gtest/gtest.h>
#include "process_killer.h"
#include "logger.h"
#include <unistd.h>
#include <fstream>
#include <filesystem>
using namespace std;

// Derived class to override the user prompt for testing.
class MockProcessKiller : public ProcessKiller {
protected:
    // Override to simulate a "Yes" response (i.e., user confirms deletion)
    bool promptUserForExecutableRemoval(const string& ransomware_path) const override {
        return true; 
    }
};

class ProcessKillerTest : public ProcessKiller, public ::testing::Test {
protected:
    ProcessKiller pk; 
};

// Test that getExecutablePath returns a non-empty string for a valid PID
TEST_F(ProcessKillerTest, GetExecutablePathValid) {
    pid_t pid = getpid();
    string path = pk.getExecutablePath(pid);
    EXPECT_FALSE(path.empty());
}

// Test that getExecutablePath returns an empty string for an invalid PID
TEST_F(ProcessKillerTest, GetExecutablePathInvalid) {
    // Use an unlikely PID
    pid_t invalid_pid = 999999;
    string path = pk.getExecutablePath(invalid_pid);
    EXPECT_TRUE(path.empty());
}

// Test that killFamily returns false for an invalid PID
TEST_F(ProcessKillerTest, KillFamilyInvalidPid) {
    pid_t invalid_pid = 999999;
    bool result = pk.killFamily(invalid_pid);
    EXPECT_FALSE(result);
}

// Test that removeExecutable returns false when given an empty path
TEST_F(ProcessKillerTest, RemoveExecutableEmptyPath) {
    bool result = pk.removeExecutable("");
    EXPECT_FALSE(result);
}

// Test that removeExecutable returns false for a non-existent file
TEST_F(ProcessKillerTest, RemoveExecutableNonExistentFile) {
    bool result = pk.removeExecutable("tests/test_files/non_existent_file.exe");
    EXPECT_FALSE(result);
}

// Test removal using a temporary file with simulated "Yes" response from user.
TEST(ProcessKillerDerivedTest, RemoveExecutableTemporaryFile_UserConfirmsDeletion) {
    MockProcessKiller testPK;
    string tempFile = "tests/test_files/temp_executable.tmp";
    
    // Create a temporary file to simulate an executable.
    ofstream ofs(tempFile);
    ofs << "dummy content";
    ofs.close();
    
    // Ensure the temporary file exists.
    EXPECT_TRUE(filesystem::exists(tempFile));
    
    // Call removeExecutable, which should now simulate the user pressing "Yes"
    bool result = testPK.removeExecutable(tempFile);
    
    // Expect the file to be successfully deleted.
    EXPECT_TRUE(result);
    EXPECT_FALSE(filesystem::exists(tempFile));
}
