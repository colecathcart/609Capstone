#include <gtest/gtest.h>
#include "entropy_calculator.h"

using namespace std;

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
};

// Test entropy for encrypted text file 
TEST_F(EntropyCalculatorTest, EncryptedTextEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_text);
    EXPECT_GT(entropy, 7) << "Expected entropy > 7 for encrypted text file";
}

// Test entropy for plaintext file 
TEST_F(EntropyCalculatorTest, PlainTextEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_text);
    EXPECT_LT(entropy, 5) << "Expected entropy < 5 for plaintext file";
}

// Test entropy for encrypted image file 
TEST_F(EntropyCalculatorTest, EncryptedImageEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_image);
    EXPECT_GT(entropy, 7) << "Expected entropy > 7 for encrypted image file";
}

// Test entropy for unencrypted image file 
TEST_F(EntropyCalculatorTest, UnencryptedImageEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_image);
    EXPECT_GT(entropy, 7) << "Expected entropy > 7 for unencrypted image file";
}

// Test entropy for encrypted ZIP file
TEST_F(EntropyCalculatorTest, EncryptedZipEntropy) {
    double entropy = calculator.get_shannon_entropy(encrypted_compressed);
    EXPECT_GT(entropy, 7) << "Expected entropy > 7 for encrypted ZIP file";
}

// Test entropy for unencrypted ZIP file
TEST_F(EntropyCalculatorTest, UnencryptedZipEntropy) {
    double entropy = calculator.get_shannon_entropy(plain_compressed);
    EXPECT_GT(entropy, 7) << "Expected entropy > 7 for unencrypted ZIP file";
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
