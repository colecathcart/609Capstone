#include "entropy_calculator.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>

typedef unsigned char BYTE;

using namespace std;

EntropyCalculator::EntropyCalculator(): buffer_size(1024) {
    logger = Logger::getInstance();
}

EntropyCalculator::EntropyCalculator(int buff_size_kb): buffer_size(buff_size_kb * 1024) {
    logger = Logger::getInstance();
}

bool EntropyCalculator::is_small_file(const string& filepath) const {
    ifstream file(filepath, ios::binary);

    if (!file.is_open()) {
        logger->log("Error opening file");
        return -1;
    }

    file.seekg(0, ios::end);
    if(file.tellg() < 1024) {
        file.close();
        return true;
    }
    file.close();
    return false;
}

bool EntropyCalculator::calc_shannon_entropy(const string& filepath, int hits) const {
    
    if(is_small_file(filepath)) {
        return 0;
    }
    
    ifstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        logger->log("Error opening file");
        return -1;
    }

    unordered_map<BYTE, size_t> frequencies;
    vector<BYTE> buffer(buffer_size);
    size_t total_bytes = 0;

    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer_size) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        total_bytes += bytes_read;
        for (size_t i = 0; i < bytes_read; i++){
            frequencies[buffer[i]]++;
        }
    }
    file.close();

    double entropy = 0.0;
    for(const auto& pair : frequencies) {
        double probability = static_cast<double>(pair.second) / total_bytes;
        entropy -= probability * log2(probability);
    }
    return entropy > 7.5;
}

bool EntropyCalculator::monobit_test(const string& filepath, int hits) const {

     if(is_small_file(filepath)) {
        return false;
    }

    ifstream file(filepath, ios::binary);

    if(!file) {
        logger->log("Error opening file");
        return 1;
    }

    vector<BYTE> buffer(buffer_size);
    double num_blocks = 0.0;
    double num_passed_blocks = 0.0;

    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer_size) || file.gcount() > 0) {
        
        streamsize bytes_read = file.gcount();
        num_blocks++;

        // not worth running monobit test on < 100 bits
        if (bytes_read < 13) {
            break;
        }
        long count_1s = 0;

        for (int i = 0; i < bytes_read; i++) {
            for (int j = 7; j >= 0; j--) {
                if(buffer[i] & (1 << j)) {
                    count_1s++;
                }
            }
        }

        long num_bits = bytes_read * 8;
        long sn = abs(count_1s - (num_bits - count_1s));
        double s_obs = sn / sqrt(num_bits);
        double p_value = erfc(s_obs / sqrt(2));

        if(p_value >= 0.01) {
            num_passed_blocks++;
        }
    }

    file.close();

    if (num_passed_blocks / num_blocks >= 0.9) {
        return true;
    }
    return false;
}

string EntropyCalculator::get_file_hash(const string& filepath) const {
    ifstream file(filepath, ios::binary);
    if (!file) {
        logger->log("Couldn't open file for generating hash");
        return "";
    }
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        logger->log("Error initializing EVP context.");
        return "";
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        logger->log("Error initializing SHA-256.");
        EVP_MD_CTX_free(ctx);
        return "";
    }

    const int bufferSize = 8192;
    char buffer[bufferSize];
    while (file.read(buffer, bufferSize)) {
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1) {
            logger->log("Error updating SHA-256 hash.");
            EVP_MD_CTX_free(ctx);
            return "";
        }
    }
    if (file.gcount() > 0) {
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1) {
            logger->log("Error updating SHA-256 hash.");
            EVP_MD_CTX_free(ctx);
            return "";
        }
    }

    file.close();

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        logger->log("Error updating SHA-256 hash.");
        EVP_MD_CTX_free(ctx);
        return "";
    }

    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();

}