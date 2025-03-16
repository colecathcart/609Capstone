#include "event.h"

using namespace std;

Event::Event(const string& type, 
            const string& path, 
            const string& name, 
            const string& ext, 
            time_t timestamp, 
            pid_t pid)
    : event_type(type), filepath(path), filename(name), extension(ext), time(timestamp), pid(pid) {}

string Event::get_event_type() const { return event_type; }
string Event::get_filepath() const { return filepath; }
string Event::get_filename() const { return filename; }
string Event::get_extension() const { return extension; }
time_t Event::get_time() const { return time; }
pid_t Event::get_pid() const { return pid; }

string Event::print() const {

    return "Event Type: " + event_type + 
            "\n\tFile: " + filename +
             "\n\tTimestamp: " + to_string(time) + 
             "\n\tPID: " + to_string(pid);
}