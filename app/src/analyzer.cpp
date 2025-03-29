#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "websocket_server.h"

#define HASHES_PATH "data/blacklist_hashes.txt"
#define IGNORE_THRESHOLD 30

using namespace std;

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker()), process_killer(ProcessKiller()), logger(Logger::getInstance())
{
}

void Analyzer::save_hash(const string& hash) const {

    ofstream file(HASHES_PATH, ios::app);
    if (!file.is_open()) {
        logger.log("Error opening file for storing hash!");
        return;
    }
    file << hash << std::endl;
    file.close();
    add_blacklisted_hash(hash); // Add hash to database
}

int Analyzer::update_watch(pid_t pid) {
    lock_guard<mutex> lock(watch_mutex);

    uint64_t start_time = get_start_time(pid);
    if(watched_procs.find(pid) != watched_procs.end()) {
        if(watched_procs[pid].time == start_time) {
            watched_procs[pid].hits += 1;
        } else {
            watched_procs[pid].time = start_time;
            watched_procs[pid].hits = 1;
        }
    } else {
        watched_procs[pid] = Process(start_time, 1);
    }
    return watched_procs[pid].hits;
}

uint64_t Analyzer::get_start_time(pid_t pid) {
    lock_guard<mutex> lock(procfile_mutex);
    string stat_file = "/proc/" + std::to_string(pid) + "/stat";
    ifstream file(stat_file);
    
    if (!file.is_open()) {
        logger.log("Unable to open stat file for PID: " + to_string(pid));
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

void Analyzer::analyze(Event& event) {

    int num_hits = update_watch(event.get_pid());

    if(num_hits > IGNORE_THRESHOLD) {
        return;
    }

    bool is_suspicious = false;
    if(file_checker.is_blacklist_extension(event.get_filename())) {
        is_suspicious = true;
    }

    if(file_checker.needs_monobit(event.get_filepath())) {
        if(calculator.monobit_test(event.get_filepath(), num_hits)) {
            is_suspicious = true;
        }
    } else if(calculator.calc_shannon_entropy(event.get_filepath(), num_hits)) {
        is_suspicious = true;
    }

    if(is_suspicious) {
        logger.log(event.print());
        logger.log("Process " + to_string(event.get_pid()) + " has been seen " + to_string(num_hits) + " times");
        logger.log("Process " + to_string(event.get_pid()) + " is suspicious, flagging for removal.");
        string exec_path = process_killer.getExecutablePath(event.get_pid());
        process_killer.killFamily(event.get_pid());

        lock_guard<mutex> lock(analyzer_mutex);
        if(exec_path != "") {
            send_stat_update("SUSPICIOUS"); // Send message to increment count for suspicious processes 
            send_stat_update("KILLED"); // Send message to increment count for processes killed
            save_hash(calculator.get_file_hash(exec_path));
            process_killer.removeExecutable(exec_path);
        }
        watched_procs.erase(event.get_pid());
        return;
    }
    logger.log(event.print());
    logger.log("Process " + to_string(event.get_pid()) + " has been seen " + to_string(num_hits) + " times");
    logger.log("Process " + to_string(event.get_pid()) + " is not suspicious.");
}