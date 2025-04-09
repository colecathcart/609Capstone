#ifndef EVENT
#define EVENT

#include <string>
#include <ctime>
#include <iostream>

using namespace std;

/**
 * @brief Data structure to hold information about filesystem events. Created by the 
 * EventDetector from fanotify events and used by the rest of the program.
 */
class Event {
private:
    /**
     * @brief The fanotify event type.
     */
    string event_type;
    /**
     * @brief The path of the file from the event.
     */
    string filepath;
    /**
     * @brief The name of the file from the event.
     */
    string filename;
    /**
     * @brief The timestamp of the event.
     */
    time_t time;
    /**
     * @brief The calling process id of the event.
     */
    pid_t pid;

public:

    /**
     * @brief Constructor with all params for all fields.
     */
    Event(const string& type, const string& path, 
          const string& name, time_t timestamp, pid_t pid);
    /**
     * @brief Default constructor.
     */
    Event();
    /**
     * @brief Returns event type.
     */
    string get_event_type() const;
    /**
     * @brief Returns filepath.
     */
    string get_filepath() const;
    /**
     * @brief Returns file name.
     */
    string get_filename() const;
    /**
     * @brief Returns time of event.
     */
    time_t get_time() const;
    /**
     * @brief Returns calling process pid.
     */
    pid_t get_pid() const;
    /**
     * @brief Returns a formatted string of event details.
     */
    string print() const;
};

#endif
