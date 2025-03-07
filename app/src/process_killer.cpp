#include "../include/process_killer.h"
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <cstdlib> // For system()

ProcessKiller::ProcessKiller() {

}

string ProcessKiller::getExecutablePath(pid_t pid) const {
    string exe_path = "/proc/" + to_string(pid) + "/exe";
    char resolved_path[PATH_MAX];
    ssize_t len = readlink(exe_path.c_str(), resolved_path, sizeof(resolved_path) - 1);

    if (len != -1) {
        resolved_path[len] = '\0'; // Null-terminate the string
        return string(resolved_path);
    } else {
        cerr << "Failed to resolve ransomware executable path for PID: " << pid << endl;
        return "";
    }
}

bool ProcessKiller::killFamily(pid_t pid) {
    // Get the PGID (process group ID) of the given PID
    pid_t pgid = getpgid(pid);
    if (pgid == -1) {
        cerr << "Failed to get PGID for PID: " << pid << endl;
        return false;
    }
    
    // Kill the entire process group
    if (kill(-pgid, SIGKILL) == 0) {
        cout << "Successfully killed process group with PGID: " << pgid << endl;
        return true;
    } else {
        cerr << "Failed to kill process group with PGID: " << pgid << endl;
        return false;
    }
}

bool ProcessKiller::removeExecutable(const string& ransomware_path) const {
    if (ransomware_path.empty()) {
        cerr << "Executable path is empty. Skipping deletion." << endl;
        return false;
    }

    // Pop-up confirmation dialog using Zenity
    string command = "zenity --question --text='Do you want to delete the ransomware executable saved in " + ransomware_path + "?' --title='Confirm Deletion'";
    int result = system(command.c_str());

    // Check if the user clicked "Yes" (Zenity returns 0 for "Yes", 1 for "No")
    if (result != 0) {
        cout << "User canceled executable deletion." << endl;
        return false;
    }

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
