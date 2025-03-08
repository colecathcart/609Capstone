#ifndef LOGGER
#define LOGGER

#include <string>
#include <fstream>

using namespace std;

/**
 * @brief A singelton logger class to be shared by all classes and used 
 * for logging to a file or the standard output
 */
class Logger {
private:
    static Logger* logger;
    ofstream logFile;
    
    Logger();
    ~Logger();

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* getInstance();

    void log(const string& message, bool to_file = false);
};

#endif