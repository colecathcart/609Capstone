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

public:
    Event(const std::string& type, const std::string& path, 
          const std::string& name, const std::string& ext, std::time_t timestamp)
        : event_type(type), filepath(path), filename(name), extension(ext), time(timestamp) {}

    std::string get_event_type() const { return event_type; }
    std::string get_filepath() const { return filepath; }
    std::string get_filename() const { return filename; }
    std::string get_extension() const { return extension; }
    std::time_t get_time() const { return time; }

    void print() const {
        std::cout << "Event Type: " << event_type << ", File: " << filepath 
                  << ", Timestamp: " << time << std::endl;
    }
};

#endif // EVENT
