// RansomwareSimulator.h
#ifndef RANSOMWARE_SIMULATOR_H
#define RANSOMWARE_SIMULATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
// #include <openssl/aes.h>
// #include "event_detector.h"
#include "file_extension_checker.h"

namespace fs = std::filesystem;

/**
 * @brief Class to simulate a ransomware attack by encrypting and decrypting files.
 * @author David Nguyen
 */
class RansomwareSimulator {
private:
    EventDetector& event_detector; ///< Reference to EventDetector to log file events.
    FileExtensionChecker& file_checker; ///< Reference to FileExtensionChecker to verify file types.
    const std::string encryption_key = "16ByteEncryptKey"; ///< AES-128 encryption key (must be 16 bytes).

    /**
     * @brief Encrypts a given file using AES-128 encryption.
     * @param filepath The path of the file to encrypt.
     */
    void encrypt_file(const std::string& filepath);

    /**
     * @brief Decrypts a given file encrypted with AES-128.
     * @param filepath The path of the file to decrypt.
     */
    void decrypt_file(const std::string& filepath);

public:
    /**
     * @brief Constructor for RansomwareSimulator.
     * @param detector Reference to an EventDetector instance.
     * @param checker Reference to a FileExtensionChecker instance.
     */
    RansomwareSimulator(EventDetector& detector, FileExtensionChecker& checker);

    /**
     * @brief Simulates a ransomware attack by encrypting all files in a target directory.
     * @param target_directory The directory where files will be encrypted.
     */
    void simulate_attack(const std::string& target_directory);

    /**
     * @brief Simulates the decryption process of previously encrypted files.
     * @param target_directory The directory containing encrypted files to decrypt.
     */
    void simulate_decryption(const std::string& target_directory);
};

#endif // RANSOMWARE_SIMULATOR_H
