#include "process_killer.h"
#include "logger.h"
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <cstdlib> // For system()

ProcessKiller::ProcessKiller() {
    logger = Logger::getInstance();
}

string ProcessKiller::getExecutablePath(pid_t pid) const {
    string exe_path = "/proc/" + to_string(pid) + "/exe";
    char resolved_path[PATH_MAX];
    ssize_t len = readlink(exe_path.c_str(), resolved_path, sizeof(resolved_path) - 1);

    if (len != -1) {
        resolved_path[len] = '\0';
        return string(resolved_path);
    } else {
        logger->log("Failed to resolve ransomware executable path for PID: " + to_string(pid));
        return "";
    }
}

bool ProcessKiller::killFamily(pid_t pid) {
    // Get the PGID (process group ID) of the given PID
    pid_t pgid = getpgid(pid);
    if (pgid == -1) {
        logger->log("Failed to get PGID for PID: " + to_string(pid));
        return false;
    }
    
    // Kill the entire process group
    if (kill(-pgid, SIGKILL) == 0) {
      
      logger->log("Successfully killed process group with PGID: " + to_string(pid));
        return true;
    } else {
        logger->log("Failed to kill process group with PGID: " + to_string(pid));
        return false;
    }
}

bool ProcessKiller::promptUserForExecutableRemoval(const string& ransomware_path) const {
    // Prompt the user for executable deletion using Zenity
    string command = "zenity --question --text='A potentially malicious executable was found at " 
                     + ransomware_path + ". Do you want to delete it?' --title='Suspicious File Detected' 2>/dev/null";
    int result = system(command.c_str());
    // Zenity returns 0 for "Yes", 1 for "No"
    return (result == 0);
}

bool ProcessKiller::removeExecutable(const string& ransomware_path) const {
    if (ransomware_path.empty()) {
        logger->log("Executable path is empty. Skipping deletion.");
        return false;
    }

    if (filesystem::exists(ransomware_path)) {
        // Prompt the user if they want to delete the executable.
        if (!promptUserForExecutableRemoval(ransomware_path)) {
            logger->log("User canceled executable deletion.");
            return false;
        }
        if (filesystem::remove(ransomware_path)) {
            logger->log("Successfully deleted ransomware executable: " + ransomware_path);
            return true;
        } else {
            logger->log("Failed to delete ransomware executable: " + ransomware_path);
            return false;
        }
    } else {
        logger->log("Ransomware executable not found: " + ransomware_path);
        return false;
    }
}