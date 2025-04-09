#ifndef ENTROPYCALCULATOR
#define ENTROPYCALCULATOR

#include <string>
#include "logger.h"
#include <unordered_map>
#include <vector>

typedef unsigned char BYTE;

using namespace std;

/**
 * @brief A utility class for calculating the entropy of a given file,
 * using shannon entropy or the NIST monobit frequency test.
 * @author Cole Cathcart
 */
class EntropyCalculator
{
    public:

        /**
         * @brief Default constructor.
         */
        EntropyCalculator();

        /**
         * @brief Function to calculate if a given file has high entropy blocks
         * @param filepath The path to the file to be tested.
         * @param hits The number of times the process has been seen, used to increase the amount of the file
         * for which entropy is calculated.
         */
        bool calc_shannon_entropy(const string& filepath, int hits) const;

        /**
         * @brief Function to perform the NIST monobit test on a given file.
         * A true result denotes that 90% or more of the blocks in the file chunk
         * (determined by hits) passed the test.
         * @param filepath The path to the file to be tested.
         * @param hits The number of times the process has been seen, used to increase the amount of the file
         * for which the test is calculated.
         */
        bool monobit_test(const string& filepath, int hits) const;

        /**
         * @brief Function to get the SHA256 hash of the file.
         * @param filepath The path to the file to be hashed.
         */
        string get_file_hash(const string& filepath) const;
    
    private:

        /**
         * @brief All valid base64 characters, along with their positions, for efficiently decoding.
         */
        static const unordered_map<BYTE, int> base64_set;
        /**
         * @brief Reference to singleton logger.
         */
        Logger& logger;
        /**
         * @brief Helper function to ignore small files (< 1 KB).
         */
        bool is_small_file(const string& filepath) const;
        /**
         * @brief Helper function to check for and perform decoding in base64.
         * Adapted from this code (open license): https://github.com/ReneNyffenegger/cpp-base64 
         */
        vector<BYTE> decode(vector<BYTE>& buffer, size_t length) const;
};

#endif