#include "entropycalculator.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cmath>

using namespace std;

EntropyCalculator::EntropyCalculator() {
    // default
}

double EntropyCalculator::get_entropy(const string& filepath) const {
    ifstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return -1;
    }

    unordered_map<unsigned char, double> frequencies;
    unsigned char byte;
    int totalBytes = 0;

    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        frequencies[byte]++;
        totalBytes++;
    }
    file.close();

    double entropy = 0.0;
    for(const auto& pair : frequencies) {
        double probability = pair.second / totalBytes;
        entropy -= probability * log2(probability);
    }
    return entropy;
}

double EntropyCalculator::get_magic_bytes_entropy(const string& filepath) const {
    ifstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return -1;
    }

    unordered_map<unsigned char, double> frequencies;
    unsigned char byte;
    int totalBytes = 0;

    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte)) && totalBytes < 4) {
        frequencies[byte]++;
        totalBytes++;
    }
    file.close();

    double entropy = 0.0;
    for(const auto& pair : frequencies) {
        double probability = pair.second / totalBytes;
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

    char byte;
    long size = 0;
    long count1 = 0;

    while (file.get(byte)) {
        size ++;
        for (int i= 7; i >= 0; i--) {
            if(byte & (1 << i)){
                count1++;
            }
        }
    }

    file.close();

    int num_bits = size*8;
    int sn = abs(count1 - (num_bits - count1));
    int s_obs = sn / sqrt(num_bits);
    double p_value = erfc(s_obs / sqrt(2));

    return !(p_value < 0.01);
}

bool EntropyCalculator::is_encrypted(double e1, double e2) const {
    return 0;
}