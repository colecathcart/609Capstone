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

bool EntropyCalculator::is_encrypted(double e1, double e2) const {
    return 0;
}