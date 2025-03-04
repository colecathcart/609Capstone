#ifndef EVENT
#define EVENT

#include <string>
#include <ctime>
#include <iostream>

class Event {
private:
    std::string event_type;
    std::string filepath;
    std::string filename;
    std::string extension;
    std::time_t time;
    pid_t pid;

public:
    Event(const std::string& type, const std::string& path, 
          const std::string& name, const std::string& ext, std::time_t timestamp, pid_t pid);

    std::string get_event_type() const;
    std::string get_filepath() const;
    std::string get_filename() const;
    std::string get_extension() const;
    std::time_t get_time() const;
    pid_t get_pid() const;

    void print() const;
};

#endif // EVENT
