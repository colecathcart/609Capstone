#ifndef ANALYZER
#define ANALYZER

#include <unordered_map>
#include <string>
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"

using namespace std;

/**
 * @brief Class to analyze events emitted by the EventDetector. Holds
 * a table of suspicious processes to track repeat offenders that need
 * to be killed
 */
class Analyzer
{
    public:

        /**
         * @brief Default constructor
         */
        Analyzer();

        /**
         * @brief Analyzes the given event, storing the process id and timestamp
         * internally if deemed suspicious.
         * @param event The event to be analyzed
         */
        void analyze(Event& event);

    private:

        /**
         * @brief A map holding identified suspicious processes
         */
        unordered_map<string, time_t> suspicious_procs;

        /**
         * @brief An instance of EntropyCalculator for determining encryption
         */
        EntropyCalculator calculator;

        /**
         * @brief An instance of FileExtensionChecker for determining file type
         */
        FileExtensionChecker file_checker;
};

#endif