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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mount point>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fanotify_fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }

    if (fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_OPEN_PERM | FAN_CLOSE_WRITE, AT_FDCWD, argv[1]) == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2] = {{.fd = STDIN_FILENO, .events = POLLIN}, {.fd = fanotify_fd, .events = POLLIN}};
    printf("Monitoring file access on %s. Press Enter to stop.\n", argv[1]);

    while (1) {
        if (poll(fds, 2, -1) > 0 && fds[0].revents & POLLIN) {
            break;
        }
        if (fds[1].revents & POLLIN) {
            handle_events(fanotify_fd);
        }
    }

    close(fanotify_fd);
    return 0;
}
