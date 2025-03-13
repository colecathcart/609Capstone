#ifndef LOGGER
#define LOGGER

#include <string>
#include <fstream>

using namespace std;

/**
 * @brief A singleton logger class to be shared by all classes and used 
 * for logging to a file or the standard output
 */
class Logger {
private:
    /**
     * @brief Singleton logger instance
     */
    static Logger* logger;
    /**
     * @brief The file to log to
     */
    ofstream logFile;
    /**
     * @brief the named pipe to log to (for communicating logs to the GUI)
     */
    int fifo;
    /**
     * @brief the path of the named pipe (for communicating logs to the GUI)
     */
    string pipePath;
    
    Logger(); // Private constructor
    
    ~Logger(); // Private destructor

public:
    Logger(const Logger&) = delete; // Remove copy constructor
    Logger& operator=(const Logger&) = delete; //Remove assignment operator

    /**
     * @brief Instantiate a pointer to the singleton instance
     */
    static Logger* getInstance();

    /**
     * @brief Log a message to a variety of outputs (stdout by default)
     * @param message The message to log
     * @param whereto Where to log the message: 0 = stdout, 1 = logfile, 2 = fifo pipe, 3 = all
     */
    void log(const string& message, int whereto = 0);
};

#endif