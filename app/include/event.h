#ifndef EVENT
#define EVENT

#include <string>
#include <ctime>
#include <iostream>

using namespace std;

class Event {
private:
    string event_type;
    string filepath;
    string filename;
    time_t time;
    pid_t pid;

public:
    Event(const string& type, const string& path, 
          const string& name, time_t timestamp, pid_t pid);

    Event();

    string get_event_type() const;
    string get_filepath() const;
    string get_filename() const;
    time_t get_time() const;
    pid_t get_pid() const;

    string print() const;
};

#endif // EVENT
