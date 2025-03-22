#include "logreader.h"
#include <QDebug>

LogReader::LogReader() {
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
}

LogReader::~LogReader() {
    if (mq != -1) {
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
    }
}

QString LogReader::receive_message() {
    memset(message, 0, MAX_SIZE);

    ssize_t bytes_read = mq_receive(mq, message, MAX_SIZE, nullptr);

    if (bytes_read == -1) {
        qInfo() << "Failed to receive message";
    }

    QString msg = QString::fromUtf8(message, bytes_read);

    if (msg == MSG_STOP) {
        qInfo() << "Received stop signal";
        return QString(MSG_STOP);
    }

    qInfo() << "Received message: " << msg;
    return msg;
}
