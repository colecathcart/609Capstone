#include "../include/process_killer.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <csignal>
#include <unistd.h>
#include <iostream>

namespace fs = std::filesystem;

ProcessKiller::ProcessKiller(int pid)
    : process_id(pid) {}

    bool ProcessKiller::killFamily() {
        // Kill the entire process group
        if (kill(-process_id, SIGKILL) == 0) {
            cout << "Successfully killed process group with PGID: " << process_id << endl;
            return true;
        } else {
            cerr << "Failed to kill process group with PGID: " << process_id << endl;
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
