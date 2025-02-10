#include "entropy_calculator.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <vector>

typedef unsigned char BYTE;

using namespace std;

EntropyCalculator::EntropyCalculator(): buffer_size(8 * 1024) {
    // default
}

EntropyCalculator::EntropyCalculator(int buff_size_kb): buffer_size(buff_size_kb * 1024) {

}

double EntropyCalculator::get_shannon_entropy(const string& filepath) const {
    ifstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
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
    return entropy;
}

bool EntropyCalculator::monobit_test(const string& filepath) const {
    ifstream file(filepath, ios::binary);

    if(!file) {
        cerr << "Error opening file" << endl;
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