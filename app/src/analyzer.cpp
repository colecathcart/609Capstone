
#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>

#define HASHES_PATH "data/blacklist_hashes.txt"
#define WHITELIST_PACKAGES_PATH "data/whitelist_packages.txt"

using namespace std;

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker()), process_killer(ProcessKiller())
{
    logger = Logger::getInstance();
    
    fstream file(WHITELIST_PACKAGES_PATH);

    if (!file) {
        logger->log("Error opening whitelist package file!");
    }

    string line;
    while (getline(file, line)) {
        safe_packages.insert(line);
    }

    file.close();
}

void Analyzer::save_hash(const string& hash) const {
    ofstream file(HASHES_PATH, ios::app);
    //cout << hash << endl;
    if (!file.is_open()) {
        logger->log("Error opening file for storing hash!");
        return;
    }

    file << hash << std::endl;
    file.close();
}

void Analyzer::update_watch(pid_t pid, time_t timestamp)
{
    if(suspicious_procs.find(pid) != suspicious_procs.end()) {
        time_t last_hit = suspicious_procs[pid];

        // Check if last hit was less than an hour ago
        if(abs(timestamp - last_hit) < 3600) {
            logger->log("Process " + to_string(pid) + " is too suspicious, flagging for removal.");
            string exec_path = process_killer.getExecutablePath(pid);
            process_killer.killFamily(pid);
            if(exec_path != "") {
                save_hash(calculator.get_file_hash(exec_path));
                process_killer.removeExecutable(exec_path);
            }
            suspicious_procs.erase(pid);
        }
    } else {
        suspicious_procs[pid] = timestamp;
    }
}

uint64_t Analyzer::get_start_time(pid_t pid) {
    string stat_file = "/proc/" + std::to_string(pid) + "/stat";
    ifstream file(stat_file);
    
    if (!file.is_open()) {
        logger->log("Unable to open stat file for PID: " + to_string(pid));
        return -1;
    }

    string line;
    getline(file, line);
    file.close();

    istringstream ss(line);
    string ignore;
    uint64_t start_time;
    
    // Reading through the file to get the start time at position 22
    ss >> ignore;  // skipping PID
    char ch;
    ss >> ch;
    while (ss.get(ch) && ch != ')') {
        // skipping over Comm
    }
    for (int i = 0; i < 20; ++i) {
        ss >> ignore;  // Skip remaining fields
    }

    ss >> start_time;
    return start_time;
}

bool Analyzer::is_trusted_process(pid_t pid) {
    uint64_t start_time = get_start_time(pid);
    bool is_trusted = false;
    if(safe_procs.find(pid) != safe_procs.end()) {
        is_trusted = start_time == safe_procs[pid];
    }

    if (!is_trusted) {
        safe_procs.erase(pid);

        string command = "dpkg-query -S " + process_killer.getExecutablePath(pid) + " 2>/dev/null";
        FILE* fp = popen(command.c_str(), "r");
        if (!fp) {
            logger->log("Couldn't run dpkg-query for package");
            return false;
        }

        char result[1024];
        if (fgets(result, sizeof(result), fp) == nullptr) {
           logger->log("Executable not from a package");
            pclose(fp);
            return false;
        }
        pclose(fp);

        string package_name(result);
        size_t pos = package_name.find(':');
        if (pos != string::npos) {
            package_name = package_name.substr(0, pos); // remove package path
        }

        if (safe_packages.find(package_name) != safe_packages.end()) {
            logger->log("Process " + to_string(pid) + "is trustworthy. Updating list.");
            safe_procs[pid] = start_time;
            return true;
        }

        command = "dpkg-query -W -f='${Origin}' " + package_name;
        fp = popen(command.c_str(), "r");
        if (!fp) {
            logger->log("Couldn't run dpkg-query for origin");
            return false;
        }
        if (fgets(result, sizeof(result), fp) == nullptr) {
            logger->log("Couldn't get package origin");
            pclose(fp);
            return false;
        }
        pclose(fp);

        string origin(result);
        //Erase whitespace
        origin.erase(origin.begin(), find_if(origin.begin(), origin.end(), [](unsigned char ch) {
            return !isspace(ch);
        }));
        origin.erase(find_if(origin.rbegin(), origin.rend(), [](unsigned char ch) {
            return !isspace(ch);  
        }).base(), origin.end());

        if(origin == "Ubuntu") {
            logger->log(package_name + "Is a safe package. Updating list.");
            safe_packages.insert(package_name);
            logger->log("Process " + to_string(pid) + "is trustworthy. Updating list.");
            safe_procs[pid] = start_time;
            return true;
        }
        return false;
    }
    return true;
}

void Analyzer::analyze(Event& event)
{
    if(is_trusted_process(event.get_pid())) {
        return;
    }
    if(file_checker.is_blacklist_extension(event.get_filename())) {
        logger->log("Process " + to_string(event.get_pid()) + " is too suspicious, flagging for removal.");
        string exec_path = process_killer.getExecutablePath(event.get_pid());
        process_killer.killFamily(event.get_pid());
        if(exec_path != "") {
            save_hash(calculator.get_file_hash(exec_path));
            process_killer.removeExecutable(exec_path);
        }
        suspicious_procs.erase(event.get_pid());
        return;
    }

    if(file_checker.needs_monobit(event.get_filepath())) {
        if(calculator.monobit_test(event.get_filepath())) {
            logger->log("Process " + to_string(event.get_pid()) + " is suspicious, updating watch.");
            update_watch(event.get_pid(), event.get_time());
            return;
        }
        logger->log("Process " + to_string(event.get_pid()) + " is not suspicious.");
        return;
    }

    if(calculator.get_shannon_entropy(event.get_filepath()) > 7.5) {
        logger->log("Process " + to_string(event.get_pid()) + " is suspicious, updating watch.");
        update_watch(event.get_pid(), event.get_time());
        return;
    }

    logger->log("Process " + to_string(event.get_pid()) + " is not suspicious.");
}