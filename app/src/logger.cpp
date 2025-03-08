
#include "logger.h"
#include <iostream>

using namespace std;

Logger* Logger::logger = nullptr;

Logger::Logger() {
    logFile.open("log.txt", ios::out | ios::app);
    if(!logFile.is_open()) {
        cerr << "Error opening log file!" << endl;
    }
}

Logger::~Logger() {
    if(logFile.is_open()) {
        logFile.close();
    }
}

Logger* Logger::getInstance() {
    if (logger == nullptr) {
        logger = new Logger();
    }
    return logger;
}

void Logger::log(const string& message, bool to_file) {
    if(to_file && logFile.is_open()) {
        logFile << message << endl;
    } else {
        cout << message << endl;
    }
}