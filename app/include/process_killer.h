#ifndef PROCESS_KILLER
#define PROCESS_KILLER

#include <string>
#include "logger.h"

using namespace std;

/**
 * @brief A class for terminating processes and cleaning up associated files.
 * This class provides functionality to kill a process and its child processes,
 * as well as delete the ransomware executable.
 */
class ProcessKiller {
private:

    /**
     * @brief Singleton logger reference
     */
    Logger& logger;
    /**
     * @brief Mutex for thread safety
     */
    mutable mutex file_mutex;

protected:

    /**
     * @brief Prompts the user before deleting the ransomware executable.
     * @param ransomware_path The path to the ransomware executable.
     */
    virtual bool promptUserForExecutableRemoval(const string& ransomware_path) const;

public:

    /**
     * @brief Constructs a ProcessKiller instance 
     */
    ProcessKiller();

    /**
     * @brief Kills the process and its process group.
     * @param pid The pid of the process to kill.
     */
    bool killFamily(pid_t pid) const;

    /**
     * @brief Retrieves the executable path of the process.
     * @param pid The process pid.
     */
    string getExecutablePath(pid_t pid) const;

    /**
     * @brief Removes the ransomware executable from the system.
     * @param ransomware_path Path to the executable file.
     */
    bool removeExecutable(const string& ransomware_path) const;

};

#endif
