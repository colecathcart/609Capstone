#ifndef ENTROPYCALCULATOR
#define ENTROPYCALCULATOR

#include <string>
#include "logger.h"

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
         * @brief Constructor.
         * @param buff_size_kb The size of buffer (in KB) to read in at a time.
         */
        EntropyCalculator(int buff_size_kb);

        /**
         * @brief Function to calculate if a given file has high entropy blocks
         * @param filepath The path to the file to be tested.
         * @param hits The number of times the process has been seen, used to increase the amount of the file
         * for which entropy is calculated.
         */
        bool calc_shannon_entropy(const string& filepath, int hits) const;

        /**
         * @brief Function to perform the NIST monobit test on a given file.
         * A true result denotes that 90% or more of the blocks in the file
         * (determined by the buffer_size) passed the test.
         * @param filepath The path to the file to be tested.
         * @param hits The number of times the process has been seen, used to increase the amount of the file
         * for which the test is calculated.
         */
        bool monobit_test(const string& filepath, int hits) const;

        /**
         * @brief Function to get the SHA256 hash of the file, for the purpose of comparison.
         * @param filepath The path to the file to be tested.
         */
        string get_file_hash(const string& filepath) const;
    
    private:

        /**
         * @brief The size of buffer (in Bytes) to be read from a file at a time
         */
        const size_t buffer_size;
        /**
         * @brief Reference to singleton logger
         */
        Logger* logger;
        /**
         * @brief Helper function to ignore small files (< 1 KB)
         */
        bool is_small_file(const string& filepath) const;

};

#endif