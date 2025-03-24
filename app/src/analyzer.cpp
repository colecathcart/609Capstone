#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"
#include "websocket_server.h"

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker()), process_killer(ProcessKiller())
{
    logger = Logger::getInstance();
}

void Analyzer::update_watch(pid_t pid, time_t timestamp)
{
    if(suspicious_procs.find(pid) != suspicious_procs.end()) {
        time_t last_hit = suspicious_procs[pid];
        
        // Check if last hit was less than an hour ago
        if(abs(timestamp - last_hit) < 3600) {
            logger->log("Process " + to_string(pid) + " is too suspicious, flagging for removal.");
            string exec_path = process_killer.getExecutablePath(pid);
            if (exec_path != "") {
                increment_suspicious(); // Increment count for suspicious processes
            }
            if (process_killer.killFamily(pid)) {
                increment_killed(); // Increment count for processes killed
            };
            process_killer.removeExecutable(exec_path);
            suspicious_procs.erase(pid);
        }
    } else {
        suspicious_procs[pid] = timestamp;
    }
}

void Analyzer::analyze(Event& event)
{
    if(file_checker.is_suspicious(event.get_filename())) {
        logger->log("Process " + to_string(event.get_pid()) + " is too suspicious, flagging for removal.");
        string exec_path = process_killer.getExecutablePath(event.get_pid());
        if (exec_path != "") {
            increment_suspicious();
        }
        if (process_killer.killFamily(event.get_pid())) {
            increment_killed();
        };
        process_killer.removeExecutable(exec_path);
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