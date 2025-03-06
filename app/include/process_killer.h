#ifndef PROCESS_KILLER
#define PROCESS_KILLER

#include <string>

using namespace std;

/**
 * @brief A class for terminating processes and cleaning up associated files.
 * This class provides functionality to kill a process and its child processes,
 * as well as delete the ransomware executable.
 * 
 * @author Rana El Sadig
 */
class ProcessKiller {
public:

    /**
     * @brief Constructs a ProcessKiller instance 
     */
    ProcessKiller();

    /**
     * @brief Kills the process and its process group.
     * @return True if the process group was successfully terminated, false otherwise.
     */
    bool killFamily(pid_t pid);

    /**
     * @brief Retrieves the executable path of the process.
     * @return The resolved path of the executable or an empty string if retrieval fails.
     */
    string getExecutablePath(pid_t pid) const;

    /**
     * @brief Removes the ransomware executable from the system.
     * @param ransomware_path Path to the executable file.
     * @return True if the executable was successfully deleted, false otherwise.
     */
    bool removeExecutable(const string& ransomware_path) const;

};

#endif // PROCESS_KILLER
