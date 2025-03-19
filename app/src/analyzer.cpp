
#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"

#define HASHES_PATH "data/blacklist_hashes.txt"

using namespace std;

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker()), process_killer(ProcessKiller())
{
    logger = Logger::getInstance();
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

void Analyzer::analyze(Event& event)
{
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