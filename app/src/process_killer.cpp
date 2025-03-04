#include "../include/process_killer.h"
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <limits.h>

bool ProcessKiller::killFamily() {
    // Get the PGID (process group ID) of the given PID
    pid_t pgid = getpgid(process_id);
    if (pgid == -1) {
        std::cerr << "Failed to get PGID for PID: " << process_id << std::endl;
        return false;
    }

    // Kill the entire process group
    if (kill(-pgid, SIGKILL) == 0) {
        std::cout << "Successfully killed process group with PGID: " << pgid << std::endl;
        return true;
    } else {
        std::cerr << "Failed to kill process group with PGID: " << pgid << std::endl;
        return false;
    }
}

bool ProcessKiller::removeExecutable() {
    // Get the executable path from /proc/[pid]/exe
    string exe_path = "/proc/" + to_string(process_id) + "/exe";
    char resolved_path[PATH_MAX];
    ssize_t len = readlink(exe_path.c_str(), resolved_path, sizeof(resolved_path) - 1);
    
    string ransomware_path;
    if (len != -1) {
        resolved_path[len] = '\0';
        ransomware_path = string(resolved_path);
    } else {
        cerr << "Failed to resolve ransomware executable path for PID: " << process_id << endl;
        return false;
    }

    // Remove the executable
    if (filesystem::exists(ransomware_path)) {
        if (filesystem::remove(ransomware_path)) {
            cout << "Successfully deleted ransomware executable: " << ransomware_path << endl;
            return true;
        } else {
            cerr << "Failed to delete ransomware executable: " << ransomware_path << endl;
            return false;
        }
    } else {
        cerr << "Ransomware executable not found: " << ransomware_path << endl;
        return false;
    }
}