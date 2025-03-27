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

const unordered_map<BYTE, int> EntropyCalculator::base64_set = {
    {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}, {'I', 8}, {'J', 9}, 
    {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17}, {'S', 18}, 
    {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23}, {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, 
    {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31}, {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, 
    {'l', 37}, {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, 
    {'u', 46}, {'v', 47}, {'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, 
    {'3', 55}, {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63}, 
    {'=', 64}
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

vector<BYTE> EntropyCalculator::decode(vector<BYTE>& buffer, size_t length) const{
    int to_count = min(100, (int)length);
    for(int i = 0; i < to_count; i++) {
        if(base64_set.find(buffer[i]) == base64_set.end()) {
            cout << "not encoded" << endl;
            return buffer;
        }
    }

    string encoded_string(buffer.begin(), buffer.begin() + 1024);

    string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    BYTE char_array_4[4], char_array_3[3];
    std::vector<BYTE> ret;

    while (in_len-- && ( encoded_string[in_] != '=')) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
        for (i = 0; i <4; i++)
            char_array_4[i] = base64_chars.find(char_array_4[i]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (i = 0; (i < 3); i++)
            ret.push_back(char_array_3[i]);
        i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
        char_array_4[j] = 0;

        for (j = 0; j <4; j++)
        char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
    }

    return ret;

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


    while (total_bytes_read < bytes_to_read && file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        size_t bytes_read = file.gcount();
        total_bytes_read += bytes_read;
        
        if (bytes_read < KILOBYTE) {
            continue;
        }

        vector<BYTE> test = decode(buffer, bytes_read);
        cout << test.data() << endl;

        for (size_t i = 0; i < bytes_read; i++) {
            frequencies[buffer[i]]++;
        }

        double entropy = 0.0;
        for(const auto& pair : frequencies) {
            double probability = static_cast<double>(pair.second) / bytes_read;
            entropy -= probability * log2(probability);
        }

        cout << "Entropy:" << entropy << endl;
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