#ifndef ANALYZER
#define ANALYZER

#include <unordered_map>
#include <string>
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"
#include "process_killer.h"
#include <cstdint>
#include <tuple>

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
         * @brief Analyzes the given event, storing the process id, creation time and
         * number of times that process has been seen.
         * @param event The event to be analyzed
         */
        void analyze(Event& event);

    private:

        /**
         * @brief Struct for holding process info for process tracking
         * @param time The start time of the process
         * @param hits The number of times this process has been seen
         */
        struct Process {
            uint64_t time;
            int hits;
        
            Process(uint64_t start_time, int num_hits) : time(start_time), hits(num_hits) {}
            Process() : time(0), hits(0) {}
        };

        /**
         * @brief A map holding trusted procs with their creation time and number of hits
         */
        unordered_map<pid_t, Process> watched_procs;
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
         * @brief Reference to singleton logger
         */
        Logger& logger;
        /**
         * @brief Mutexes for thread safety
         */
        mutable mutex analyzer_mutex, watch_mutex, procfile_mutex;
        /**
         * @brief Function to update the process watchlist for the given pid
         * @return The number of times this process has been seen.
         */
        int update_watch(pid_t pid);
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