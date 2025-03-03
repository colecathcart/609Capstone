
#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker())
{
}

void Analyzer::update_watch(pid_t pid, time_t timestamp)
{
    if(suspicious_procs.find(pid) != suspicious_procs.end()) {
        time_t last_hit = suspicious_procs[pid];

        // Check if last hit was less than an hour ago
        if(abs(timestamp - last_hit) < 3600) {
            // Replace with call to ProcessKiler
            cout << "Process " + to_string(pid) + "is too suspicious, flagging for removal." << endl;
            
            suspicious_procs.erase(pid);
        }
    } else {
        suspicious_procs[pid] = timestamp;
    }
}

void Analyzer::analyze(Event& event)
{
    if(file_checker.is_suspicious(event.get_filename())) {
        // replace with call to ProcessKiller
        cout << "Process " + to_string(event.get_pid()) + "is too suspicious, flagging for removal." << endl;
        return;
    }

    if(file_checker.is_compressed(event.get_filename()) || file_checker.is_image(event.get_filename())) {
        if(calculator.monobit_test(event.get_filepath())) {
            cout << "Process" + to_string(event.get_pid()) + "is suspicious, updating watch." << endl;
            update_watch(event.get_pid(), event.get_time());
            return;
        }
        cout << "Process" + to_string(event.get_pid()) + "is not suspicious." << endl;
        return;
    }

    if(calculator.get_shannon_entropy(event.get_filepath()) > 7.5) {
        cout << "Process" + to_string(event.get_pid()) + "is suspicious, updating watch." << endl;
        update_watch(event.get_pid(), event.get_time());
    }

    cout << "Process" + to_string(event.get_pid()) + "is not suspicious." << endl;
}