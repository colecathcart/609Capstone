#ifndef LOGGER
#define LOGGER

#include <string>
#include <fstream>
#include <mqueue.h>
#include <mutex>

using namespace std;

/**
 * @brief A singleton logger class to be shared by all classes and used 
 * for logging to a file or the standard output
 */
class Logger {
private:
    /**
     * @brief The file to log to
     */
    ofstream logFile;
    /**
     * @brief The message queue to log to (for communicating logs to the GUI)
     */
    mqd_t mq;
    /**
     * @brief Where to send the logs to: 0 = stdout, 1 = logfile, 2 = fifo pipe, 3 = all
     */
    int whereto;
    /**
     * @brief Logger mutex for thread safety
     */
    mutex logger_mutex;
    
    Logger(); // Private constructor
    
    ~Logger(); // Private destructor

public:
    Logger(const Logger&) = delete; // Remove copy constructor
    Logger& operator=(const Logger&) = delete; //Remove assignment operator

    /**
     * @brief Thread-safe "Meyers' Singleton" getter function
     */
    static Logger& getInstance();

    /**
     * @brief Set the location of all future logs
     */
    void set_destination(int destination);

    /**
     * @brief Log a message to a variety of outputs (stdout by default)
     * @param message The message to log
     */
    void log(const string& message);
};

#endif
