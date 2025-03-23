#include "logreader.h"
#include <QDebug>
#include <cstring>
#include <cerrno>

LogReader::LogReader() {
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
    if (mq == (mqd_t)-1) {
        qWarning() << "Failed to open message queue: " << strerror(errno);
    }

    if (mq_getattr(mq, &attr) == -1) {
        qWarning() << "Failed to get queue attributes: " << strerror(errno);
    }
}

LogReader::~LogReader() {
    if (mq != -1) {
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
    }
}

QString LogReader::receive_message() {
    char* buffer = new char[attr.mq_msgsize];
    memset(buffer, 0, attr.mq_msgsize);

    ssize_t bytes_read = mq_receive(mq, buffer, attr.mq_msgsize, nullptr);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0'; // Null-terminate
        QString msg = QString::fromUtf8(buffer, bytes_read);
        delete[] buffer;

        msg.remove(QChar(0));
        qInfo() << "Received message: " << msg;
        return msg;
    } else {
        if (errno == EAGAIN) {
            qInfo() << "No messages available.";
        } else {
            qWarning() << "Failed to receive message: " << strerror(errno);
        }
        delete[] buffer;
        return QString();
    }
}
