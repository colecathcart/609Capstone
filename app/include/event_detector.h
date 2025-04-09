#ifndef EVENT_DETECTOR_H
#define EVENT_DETECTOR_H

#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <ctime>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/fanotify.h>
#include <unistd.h>
#include <linux/limits.h>
#include "event.h"
#include "analyzer.h"
#include "thread_pool.h"

using namespace std;

/**
 * @brief Class to monitor file system events using fanotify.
 */
class EventDetector {

public:
    /**
     * @brief Constructor initializes fanotify.
     */
    EventDetector();
    
    /**
     * @brief Destructor closes fanotify file descriptor.
     */
    ~EventDetector();

    /**
     * @brief Adds a watch on the specified path for file events.
     * @param path The directory or file path to monitor.
     */
    void add_watch(const string &path);

    /**
     * @brief Processes incoming fanotify events and extracts relevant information.
     * @param pool The thread pool to send events to for processing.
     * @param running Atomic switch to shut down threads.
     */
    void process_events(ThreadPool& pool, atomic<bool>& running);

    /**
     * @brief Retrieves the fanotify file descriptor.
     * @return The fanotify file descriptor.
     */
    int get_fanotify_fd() const;

private:
    /**
     * @brief File descriptor for fanotify instance
     */
    int fanotify_fd;
    /**
     * @brief Return value for system calls
     */
    int ret;
    /**
     * @brief Reference to singleton logger
     */
    Logger& logger;
    /**
     * @brief Set of whitelisted directories
     */
    unordered_set<string> whitelist_dirs;

    /**
     * @brief Checks if the specified path is hidden.
     * @param path The path to check.
     * @return True if the path is hidden, false otherwise.
     */
    bool is_hidden_path(const string &path);

    /**
     * @brief Checks if the specified path is allowlisted.
     * @param path The path to check.
     * @return True if the path is not a concern, false otherwise.
     */
    bool is_whitelist_path(const string &path);
};

#endif
