#include "process_killer.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

ProcessKiller::ProcessKiller(const string& name, int pid)
    : process_name(name), process_id(pid) {}

bool ProcessKiller::killFamily() {
    string command = "pkill -P " + to_string(process_id);
    if (system(command.c_str()) != 0) {
        cerr << "Failed to kill child processes for PID: " << process_id << endl;
        return false;
    }

    if (kill(process_id, SIGKILL) == 0) {
        cout << "Successfully killed process with PID: " << process_id << endl;
        return true;
    } else {
        cerr << "Failed to kill process with PID: " << process_id << endl;
        return false;
    }
}

bool ProcessKiller::deleteFiles(const vector<string>& file_paths) {
    bool all_deleted = true;

    for (const auto& path : file_paths) {
        if (fs::remove(path)) {
            cout << "Deleted file: " << path << endl;
        } else {
            cerr << "Failed to delete file: " << path << endl;
            all_deleted = false;
        }
    }

    return all_deleted;
}
