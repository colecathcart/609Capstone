
#include "logger.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

Logger* Logger::logger = nullptr;

Logger::Logger() {
    pipePath = "src/logpipe";
    fifo = open(pipePath.c_str(), O_WRONLY);
    logFile.open("log.txt", ios::out | ios::app);
    if(!logFile.is_open()) {
        cerr << "Error opening log file!" << endl;
    }
    if(fifo == -1) {
        cerr << "Error opening FIFO pipe!" << endl;
    }
}

Logger::~Logger() {
    if(logFile.is_open()) {
        logFile.close();
    }
    close(fifo);
}

Logger* Logger::getInstance() {
    if (logger == nullptr) {
        logger = new Logger();
    }
    return logger;
}

void Logger::log(const string& message, int whereto) {
    if(whereto == 0) {
        cout << message << endl;
    }else if(whereto == 1) {
        if(logFile.is_open()) {
            logFile << message << endl;
        }
    } else if(whereto == 2) {
        string guimsg = message + '\n';
        write(fifo, guimsg.c_str(), message.length());
    } else if(whereto == 3) {
        cout << message << endl;
        if(logFile.is_open()) {logFile << message << endl;}
        string guimsg = message + '\n';
        write(fifo, guimsg.c_str(), message.length());
    }
}