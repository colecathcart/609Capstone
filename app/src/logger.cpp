
#include "logger.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define MAX_SIZE 1024

using namespace std;

Logger::Logger(): whereto(0) {
    const char* mqPath = "/guiMQ";
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(mqPath, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        cerr << "Error opening message queue!" << endl;
    }

    logFile.open("log.txt", ios::out | ios::app);
    if(!logFile.is_open()) {
        cerr << "Error opening log file!" << endl;
    }
}

Logger::~Logger() {
    if(logFile.is_open()) {
        logFile.close();
    }
    mq_close(mq);
}

Logger& Logger::getInstance() {

    static Logger instance;
    return instance;
}

void Logger::set_destination(int where) {
    lock_guard<mutex> lock(logger_mutex);
    whereto = where;
}

void Logger::log(const string& message) {
    lock_guard<mutex> lock(logger_mutex);
    if(whereto == 0) {
        cout << message << endl;
    }else if(whereto == 1) {
        if(logFile.is_open()) {
            logFile << message << endl;
        }
    } else if(whereto == 2) {
        string guimsg = message;
        mq_send(mq, guimsg.c_str(), guimsg.size() + 1, 0);
    } else if(whereto == 3) {
        cout << message << endl;
        if(logFile.is_open()) {logFile << message << endl;}
        string guimsg = message;
        mq_send(mq, guimsg.c_str(), guimsg.size() + 1, 0);
    }
}