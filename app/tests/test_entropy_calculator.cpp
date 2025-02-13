#include <gtest/gtest.h>
#include "entropy_calculator.h"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

// Helper function to get entropy using the `ent` command
static double get_entropy_with_ent(const std::string& filepath) {
    string command = "ent " + filepath + " | grep Entropy | awk '{print $3}'";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return -1;
    char buffer[128];
    string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return stod(result);
}

// Create a test fixture for FileExtensionChecker
class EntropyCalculatorTest : public ::testing::Test {
    protected:
        EntropyCalculator calculator;  // Instance to be used in all tests
        string filepath = "test_files.zip";
    };

// Test for Shannon Entropy Calculation
TEST_F(EntropyCalculatorTest, ShannonEntropyTest) {
    double calculated_entropy = calculator.get_shannon_entropy(filepath);
    double expected_entropy = get_entropy_with_ent(filepath);

    ASSERT_NEAR(calculated_entropy, expected_entropy, 0.05);
}

// Test for Monobit Test
TEST_F(EntropyCalculatorTest, MonobitTest) {
    bool result = calculator.monobit_test(filepath);
    ASSERT_FALSE(result);
}