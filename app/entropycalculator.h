#ifndef ENTROPYCALCULATOR
#define ENTROPYCALCULATOR

#include <string>

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
         * @brief Function to calculate the shannon entropy of a given file.
         * @param filepath The path to the file to be tested.
         */
        double get_shannon_entropy(const string& filepath) const;

        /**
         * @brief Function to perform the NIST monobit test on a given file.
         * A true result denotes that 90% or more of the blocks in the file
         * (determined by the buffer_size) passed the test.
         * @param filepath The path to the files to be tested.
         */
        bool monobit_test(const string& filepath) const;
    
    private:

        /**
         * @brief The size of buffer (in Bytes) to be read from a file at a time
         */
        const size_t buffer_size;
};

#endif