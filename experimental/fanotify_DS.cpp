#define _GNU_SOURCE     /* Needed to get O_LARGEFILE definition */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fanotify.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <ctime>
#include <cmath>

#define BUFFER_SIZE 8192

std::map<std::string, time_t> modified_files;

// Function to check if the file has a suspicious extension
bool is_suspicious_extension(const char* filename) {
return (strstr(filename, ".locked") || strstr(filename, ".encrypted") || strstr(filename, ".crypt") || strstr(filename, ".ransomed"));
}

// Function to calculate entropy
double calculate_entropy(const char* filename) {
FILE* file = fopen(filename, "rb");
if (!file) return -1;

unsigned char buffer[1024];
int count[256] = {0};
int total_bytes = 0;

size_t bytes_read;
while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
        count[buffer[i]]++;
        total_bytes++;
    }
}
fclose(file);

double entropy = 0;
for (int i = 0; i < 256; i++) {
    if (count[i] > 0) {
        double p = (double)count[i] / total_bytes;
        entropy -= p * log2(p);
    }
}
return entropy;
}


void handle_events(int fanotify_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t len = read(fanotify_fd, buffer, sizeof(buffer));
    if (len <= 0) return;

    struct fanotify_event_metadata *metadata = (struct fanotify_event_metadata *)buffer;
    while (FAN_EVENT_OK(metadata, len)) {
        if (metadata->mask & FAN_OPEN_PERM) {
            struct fanotify_response response = {metadata->fd, FAN_ALLOW};
            if (is_suspicious_extension("/proc/self/fd/")) {
                response.response = FAN_DENY;
                printf("Suspicious file access blocked!\n");
            }
            write(fanotify_fd, &response, sizeof(response));
        }
        else if (metadata->mask & FAN_CLOSE_WRITE) {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "/proc/self/fd/%d", metadata->fd);
            char resolved_path[PATH_MAX];
            if (readlink(path, resolved_path, sizeof(resolved_path) - 1) != -1) {
                printf("Modified file: %s\n", resolved_path);
                double entropy = calculate_entropy(resolved_path);
                if (entropy > 7.5) {
                    printf("WARNING: High entropy file detected: %s\n", resolved_path);
                }
            }
        }
        close(metadata->fd);
        metadata = FAN_EVENT_NEXT(metadata, len);
    }
}

int main(int argc, char *argv[])
{
    char buf;
    int fd, poll_num;
    nfds_t nfds;
    struct pollfd fds[2];

    /* Check mount point is supplied. */

    if (argc != 2) {
        fprintf(stderr, "Usage: %s MOUNT\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Press enter key to terminate.\n");

    /* Create the file descriptor for accessing the fanotify API. */

    fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK,
                        O_RDONLY | O_LARGEFILE);
    if (fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }

    /* Mark the mount for:
        - permission events before opening files
        - notification events after closing a write-enabled
        file descriptor. */

    if (fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                        FAN_OPEN_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                        argv[1]) == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    }

    /* Prepare for polling. */

    nfds = 2;

    fds[0].fd = STDIN_FILENO;       /* Console input */
    fds[0].events = POLLIN;

    fds[1].fd = fd;                 /* Fanotify input */
    fds[1].events = POLLIN;

    /* This is the loop to wait for incoming events. */

    printf("Listening for events.\n");

    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)     /* Interrupted by a signal */
                continue;           /* Restart poll() */

            perror("poll");         /* Unexpected error */
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {

                /* Console input is available: empty stdin and quit. */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {

                /* Fanotify events are available. */

                handle_events(fd);
            }
        }
    }

    printf("Listening for events stopped.\n");
    exit(EXIT_SUCCESS);
}
