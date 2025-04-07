#include <gtest/gtest.h>
#include "entropy_calculator.h"
#include <chrono>
#include <iostream>
#include <fstream>

using namespace std::chrono;

// Test fixture for EntropyCalculator
class EntropyCalculatorTest : public ::testing::Test {
    protected:
        EntropyCalculator calculator;
        string encrypted_text = "tests/test_files/encrypted_text.txt";
        string plain_text = "tests/test_files/plain_text.txt";
        string encrypted_image = "tests/test_files/encrypted_image.jpg";
        string plain_image = "tests/test_files/plain_image.jpg";
        string plain_compressed = "tests/test_files/plain_compressed.zip";
        string encrypted_compressed = "tests/test_files/encrypted_compressed.zip";
        string large_test_file = "tests/test_files/large_test_file.bin";
};

// Test entropy for encrypted text file 
TEST_F(EntropyCalculatorTest, EncryptedTextEntropy) {
    bool result = calculator.calc_shannon_entropy(encrypted_text, 1);
    ASSERT_TRUE(result) << "Expected true for encrypted text file";
}

// Test entropy for plaintext file 
TEST_F(EntropyCalculatorTest, PlainTextEntropy) {
    bool result = calculator.calc_shannon_entropy(plain_text, 1);
    ASSERT_FALSE(result) << "Expected false for plain text file";
}

// Test entropy for encrypted image file 
TEST_F(EntropyCalculatorTest, EncryptedImageEntropy) {
    bool result = calculator.calc_shannon_entropy(encrypted_image, 1);
    ASSERT_TRUE(result) << "Expected true for encrypted image file";
}

// Test entropy for unencrypted image file 
TEST_F(EntropyCalculatorTest, UnencryptedImageEntropy) {
    bool result = calculator.calc_shannon_entropy(plain_image, 1);
    ASSERT_TRUE(result) << "Expected true for unencrypted image file";
}

// Test entropy for encrypted ZIP file
TEST_F(EntropyCalculatorTest, EncryptedZipEntropy) {
    bool result = calculator.calc_shannon_entropy(encrypted_compressed, 1);
    ASSERT_TRUE(result) << "Expected true for encrypted ZIP file";
}

// Test entropy for unencrypted ZIP file
TEST_F(EntropyCalculatorTest, UnencryptedZipEntropy) {
    bool result = calculator.calc_shannon_entropy(plain_compressed, 1);
    ASSERT_TRUE(result) << "Expected true for unencrypted ZIP file";
}

// Test monobit result for encrypted zip 
TEST_F(EntropyCalculatorTest, MonobitTestEncryptedZip) {
    bool result = calculator.monobit_test(encrypted_compressed, 1);
    ASSERT_TRUE(result) << "Expected monobit test to pass for encrypted ZIP file";
}

// Test monobit result for unencrypted zip
TEST_F(EntropyCalculatorTest, MonobitTestUnencryptedZip) {
    bool result = calculator.monobit_test(plain_compressed, 1);
    ASSERT_FALSE(result) << "Expected monobit test to fail for unencrypted ZIP file";
}

// Benchmark test for 100mb file entropy calculation
TEST_F(EntropyCalculatorTest, LargeFileEntropyBenchmark) {
    auto start = high_resolution_clock::now();
    double entropy = calculator.calc_shannon_entropy(large_test_file, 5);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start).count();

    double file_size_mb = 100;

    double speed = file_size_mb / (duration / 1000.0);

    cout << "File entropy calculation based on first 100mb of a file took " << duration << " ms (" << speed << " MB/s)" << endl;

    EXPECT_GT(entropy, 0) << "Entropy calculation should complete successfully";
}

TEST_F(EntropyCalculatorTest, SmallFileIsIgnored) {
    string small_file = plain_text;

    bool result = calculator.calc_shannon_entropy(small_file, 1);
    EXPECT_FALSE(result) << "Small file should be ignored and return false";
}


TEST_F(EntropyCalculatorTest, FileHashGeneration) {
    string hash = calculator.get_file_hash(encrypted_text);
    EXPECT_FALSE(hash.empty()) << "Hash should not be empty for valid file";
    EXPECT_EQ(hash.length(), 64) << "SHA256 hash should be 64 hex characters";
}

