#ifndef ANALYZER
#define ANALYZER

#include <unordered_map>
#include <string>
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"
#include "process_killer.h"
#include <cstdint>

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
        unordered_map<pid_t, time_t> suspicious_procs;

        /**
         * @brief A map holding trusted procs with their creation time in clock ticks
         */
        unordered_map<pid_t, uint64_t> safe_procs;

        /**
         * @brief A set holding safe or whitelisted packages
         */
        unordered_set<string> safe_packages;

        /**
         * @brief An instance of EntropyCalculator for determining encryption
         */
        EntropyCalculator calculator;

        /**
         * @brief An instance of FileExtensionChecker for determining file type
         */
        FileExtensionChecker file_checker;

        /**
         * @brief An instance of ProcessKiller for ending suspicous processes
         */
        ProcessKiller process_killer;

        /**
         * @brief reference to singleton logger
         */
        Logger* logger;

        /**
         * @brief Helper function to update suspicious_procs and take action if required
         */
        void update_watch(pid_t pid, time_t timestamp);

        /**
         * @brief Logic to determine if a process is trusted, or previously trusted, and update
         * the safe_procs map accordingly
         * @param pid the process ID to check
         */
        bool is_trusted_process(pid_t pid);

        /**
         * @brief Helper function to save hash to a file
         */
        void save_hash(const string& hash) const;

        /**
         * @brief Helper function to get process start time
         */
        uint64_t get_start_time(pid_t pid);

};

#endif