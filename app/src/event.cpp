#include "event.h"

Event::Event(const std::string& type, const std::string& path, 
        const std::string& name, const std::string& ext, std::time_t timestamp, pid_t pid)
    : event_type(type), filepath(path), filename(name), extension(ext), time(timestamp), pid(pid) {}

std::string Event::get_event_type() const { return event_type; }
std::string Event::get_filepath() const { return filepath; }
std::string Event::get_filename() const { return filename; }
std::string Event::get_extension() const { return extension; }
std::time_t Event::get_time() const { return time; }
pid_t Event::get_pid() const { return pid; }

void Event::print() const {
    std::cout << "Event Type: " << event_type << "\n\tFile: " << filename 
                << "\n\tTimestamp: " << time << ",\n\tPID: " << pid << std::endl;
}