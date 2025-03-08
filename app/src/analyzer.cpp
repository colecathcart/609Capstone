
#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker()), process_killer(ProcessKiller())
{
}

void Analyzer::update_watch(pid_t pid, time_t timestamp)
{
    if(suspicious_procs.find(pid) != suspicious_procs.end()) {
        time_t last_hit = suspicious_procs[pid];

        // Check if last hit was less than an hour ago
        if(abs(timestamp - last_hit) < 3600) {
            cout << "Process " + to_string(pid) + " is too suspicious, flagging for removal." << endl;
            string exec_path = process_killer.getExecutablePath(pid);
            process_killer.killFamily(pid);
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
        cout << "Process " + to_string(event.get_pid()) + " is too suspicious, flagging for removal." << endl;
        string exec_path = process_killer.getExecutablePath(event.get_pid());
        process_killer.killFamily(event.get_pid());
        process_killer.removeExecutable(exec_path);
        return;
    }

    if(file_checker.is_compressed(event.get_filename()) || file_checker.is_image(event.get_filename())) {
        if(calculator.monobit_test(event.get_filepath())) {
            cout << "Process " + to_string(event.get_pid()) + " is suspicious, updating watch." << endl;
            update_watch(event.get_pid(), event.get_time());
            return;
        }
        cout << "Process " + to_string(event.get_pid()) + " is not suspicious." << endl;
        return;
    }

    if(calculator.get_shannon_entropy(event.get_filepath()) > 7.5) {
        cout << "Process " + to_string(event.get_pid()) + " is suspicious, updating watch." << endl;
        update_watch(event.get_pid(), event.get_time());
        return;
    }

    cout << "Process " + to_string(event.get_pid()) + " is not suspicious." << endl;
}