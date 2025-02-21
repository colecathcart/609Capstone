#ifndef PROCESS_KILLER_H
#define PROCESS_KILLER_H

#include <string>
#include <vector>

using namespace std;

/**
 * @brief A class for terminating processes and cleaning up associated files.
 * 
 * This class provides functionality to kill a process and its child processes,
 * as well as delete related files from the system.
 */
class ProcessKiller {
public:
    /**
     * @brief Name of the process.
     */
    string process_name;

    /**
     * @brief ID of the process.
     */
    int process_id;

    /**
     * @brief Constructs a ProcessKiller instance with a process name and ID.
     * @param name Name of the process.
     * @param pid ID of the process.
     */
    ProcessKiller(const string& name, int pid);

    /**
     * @brief Kills the process and all its child processes.
     * @return True if the process and its family were successfully terminated, false otherwise.
     */
    bool killFamily();

private:
    /**
     * @brief Deletes files associated with the process.
     * @param file_paths A list of file paths to be deleted.
     * @return True if all files were successfully deleted, false otherwise.
     */
    bool deleteFiles(const vector<string>& file_paths);
};

#endif 
