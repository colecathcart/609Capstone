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
        string encrypted_text = "tests/test_files/encrypted_text.txt.gpg";
        string plain_text = "tests/test_files/plain_text.txt";
        string encrypted_image = "tests/test_files/encrypted_image.jpg.gpg";
        string plain_image = "tests/test_files/plain_image.jpg";
        string plain_compressed = "tests/test_files/plain_compressed.zip";
        string encrypted_compressed = "tests/test_files/encrypted_compressed.zip.gpg";
        string large_test_file = "tests/test_files/large_test_file.bin";
};

// Test entropy for encrypted text file 
TEST_F(EntropyCalculatorTest, EncryptedTextEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_text);
    EXPECT_NEAR(entropy, 0.0, 0.05) << "Expected entropy ~7.415 for encrypted text file";
}

// Test entropy for plaintext file 
TEST_F(EntropyCalculatorTest, PlainTextEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_text);
    EXPECT_NEAR(entropy, 4.245271, 0.05) << "Expected entropy ~4.245 for plaintext file";
}

// Test entropy for encrypted image file 
TEST_F(EntropyCalculatorTest, EncryptedImageEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_image);
    EXPECT_NEAR(entropy, 7.986892, 0.05) << "Expected entropy ~7.987 for encrypted image file";
}

// Test entropy for unencrypted image file 
TEST_F(EntropyCalculatorTest, UnencryptedImageEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_image);
    EXPECT_NEAR(entropy, 7.93159, 0.05) << "Expected entropy ~7.932 for unencrypted image file";
}

// Test entropy for encrypted ZIP file
TEST_F(EntropyCalculatorTest, EncryptedZipEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_compressed);
    EXPECT_NEAR(entropy, 7.999985, 0.05) << "Expected entropy ~7.999 for encrypted ZIP file";
}

// Test entropy for unencrypted ZIP file
TEST_F(EntropyCalculatorTest, UnencryptedZipEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_compressed);
    EXPECT_NEAR(entropy, 7.997735, 0.05) << "Expected entropy ~7.998 for unencrypted ZIP file";
}

// Test monobit result for encrypted image 
TEST_F(EntropyCalculatorTest, MonobitTestEncryptedImage) {
    bool result = calculator.monobit_test(encrypted_image);
    ASSERT_TRUE(result) << "Expected monobit test to pass for encrypted image";
}

// Test monobit result for unencrypted image
TEST_F(EntropyCalculatorTest, MonobitTestUnencryptedImage) {
    bool result = calculator.monobit_test(plain_image);
    ASSERT_FALSE(result) << "Expected monobit test to fail for unencrypted image";
}

// Benchmark test for large file entropy calculation
TEST_F(EntropyCalculatorTest, LargeFileEntropyBenchmark) {
    auto start = high_resolution_clock::now();
    double entropy = calculator.get_shannon_entropy(large_test_file);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start).count();

    ifstream file(large_test_file, ios::binary | ios::ate);
    double file_size_mb = file.tellg() / (1024.0 * 1024.0);
    file.close();

    double speed = file_size_mb / (duration / 1000.0);

    cout << "Large file entropy calculation took " << duration << " ms (" << speed << " MB/s)" << endl;

    EXPECT_GT(entropy, 0) << "Entropy calculation should complete successfully";
}
