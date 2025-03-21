#ifndef LOGREADER_H
#define LOGREADER_H

#include <mqueue.h>
#include <fcntl.h>
#include <QString>

#define QUEUE_NAME "/guiMQ"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

class LogReader
{
public:
    LogReader();
    ~LogReader();
    QString receive_message();

private:
    mqd_t mq;
    struct mq_attr attr;
    char message[MAX_SIZE];

};

#endif // LOGREADER_H
