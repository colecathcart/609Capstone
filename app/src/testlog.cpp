#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/stat.h>

#define QUEUE_NAME "/guiMQ"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char message[MAX_SIZE];

    // Set the message queue attributes (these can be default if we're just reading)
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Open the message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // Receive messages from the queue
    while (true) {
        ssize_t bytes_read = mq_receive(mq, message, MAX_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            return 1;
        }

        // Print the received message
        std::cout << "Received: " << message << std::endl;

        // If the message is "exit", stop reading
        if (strcmp(message, MSG_STOP) == 0) {
            break;
        }
    }

    std::cout << "Window finished receiving messages." << std::endl;

    // Close the message queue
    mq_close(mq);

    // Optionally unlink the queue after we're done
    mq_unlink(QUEUE_NAME);

    return 0;
}