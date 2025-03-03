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

/**
 * @brief Class to monitor file system events using fanotify.
 */
class EventDetector {
private:
    int fanotify_fd; ///< File descriptor for fanotify instance
    int ret;         ///< Return value for system calls
    std::queue<Event> event_queue; ///< Queue to store recent events
    size_t max_queue_size = 100; ///< Maximum number of stored events
    Analyzer analyzer; ///< For passing events to

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
    void add_watch(const std::string &path);

    /**
     * @brief Processes incoming fanotify events and extracts relevant information.
     */
    void process_events();

    /**
     * @brief Logs event details to the console.
     * @param event The event to be logged.
     */
    void log_event(const Event &event);

    /**
     * @brief Stores an event in the event queue, maintaining a fixed queue size.
     * @param event The event to be stored.
     */
    void enqueue_event(const Event &event);

    /**
     * @brief Retrieves the fanotify file descriptor.
     * @return The fanotify file descriptor.
     */

    int get_fanotify_fd() const;

    /**
     * @brief Checks if the specified path is hidden.
     * @param path The path to check.
     * @return True if the path is hidden, false otherwise.
     */
    bool is_hidden_path(const std::string &path);
};

#endif // EVENT_DETECTOR_H
