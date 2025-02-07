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
#include <sys/fanotify.h>
#include <unistd.h>
#include <linux/limits.h>

/**
 * @brief Structure to store file event details.
 */
struct Event {
    std::string event_type; ///< Type of event (write or delete)
    std::string filepath;   ///< Full path of the affected file
    std::string filename;   ///< Name of the affected file
    std::string extension;  ///< File extension
    std::time_t time;       ///< Timestamp of the event
};

/**
 * @brief Class to monitor file system events using fanotify.
 */
class EventDetector {
private:
    int fanotify_fd; ///< File descriptor for fanotify instance
    std::queue<Event> event_queue; ///< Queue to store recent events
    size_t max_queue_size = 100; ///< Maximum number of stored events

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
};

#endif // EVENT_DETECTOR_H
