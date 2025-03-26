#include "entropy_calculator.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>

#define MEGABYTE 1024 * 1024
#define KILOBYTE 1024

typedef unsigned char BYTE;

using namespace std;

static const unordered_set<BYTE> base64_set = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '='
};

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

vector<BYTE> EntropyCalculator::decode(vector<BYTE>& buffer, size_t length){
    int to_count = min(100, (int)length);
    for(int i = 0; i < to_count; i++) {
        if(base64_set.find(buffer[i]) == base64_set.end()) {
            return buffer;
        }
    }

    BIO *bio, *base64;
    int decode_len = static_cast<int>(buffer.size());
    vector<BYTE> decoded_buffer(decode_len);

    base64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(buffer.data(), decode_len);
    bio = BIO_push(base64, bio);

    int length = BIO_read(bio, decoded_buffer.data(), decode_len);
    decoded_buffer.resize(length);

    return decoded_buffer;

}

bool EntropyCalculator::calc_shannon_entropy(const string& filepath, int hits) const {
    
    if(is_small_file(filepath)) {
        return false;
    }

    ifstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        logger->log("Error opening file");
        return -1;
    }

    int mbs_to_read = 0;
    switch (hits)
    {
    case 1:
        mbs_to_read = 1;
        break;
    case 2:
        mbs_to_read = 10;
        break;
    case 3:
        mbs_to_read = 20;
        break;
    case 4:
        mbs_to_read = 50;
        break;
    default:
        mbs_to_read = 100;
        break;
    }

    unordered_map<BYTE, size_t> frequencies;
    vector<BYTE> buffer(MEGABYTE);
    size_t total_bytes_read = 0;
    size_t bytes_to_read = mbs_to_read * MEGABYTE;


    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        size_t bytes_read = file.gcount();
        total_bytes_read += bytes_read;
        
        if (bytes_read < KILOBYTE) {
            continue;
        }

        for (size_t i = 0; i < bytes_read; i++) {
            frequencies[buffer[i]]++;
        }

        double entropy = 0.0;
        for(const auto& pair : frequencies) {
            double probability = static_cast<double>(pair.second) / bytes_read;
            entropy -= probability * log2(probability);
        }

        cout << entropy << endl;
        if (entropy > 7.5) {
            file.close();
            return true;
        }

        frequencies.clear();
    }
    file.close();
    return false;
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

    cout << num_passed_blocks << " / " << num_blocks << " blocks passed" << endl;

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