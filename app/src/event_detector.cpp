#include "event_detector.h"
#include <iostream>
#include <string>
#include <ctime>
#include <queue>
#include <unistd.h>
#include <fcntl.h>
#include <sys/fanotify.h>
#include <limits.h>
#include <errno.h>
#include <sstream>

#define WHITELIST_DIRS_PATH "data/whitelist_dirs.txt"

using namespace std;

EventDetector::EventDetector(): logger(Logger::getInstance()) {
    logger.log( "Initializing EventDetector...");

    ifstream file(WHITELIST_DIRS_PATH);
    string extension;

    if (file.is_open()) {
        while (getline(file, extension)) {
            if (!extension.empty()) {
                whitelist_dirs.insert(extension);
            }
        }
        file.close();
    } else {
        logger.log("Error opening whitelist dirs file");
    }   

    // Initialize fanotify file descriptor
    fanotify_fd = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
    logger.log("fanotify_fd initialized with FD: " + to_string(fanotify_fd));
}

EventDetector::~EventDetector() {
    logger.log("Closing fanotify_fd: " + to_string(fanotify_fd));
    close(fanotify_fd);
}

void EventDetector::add_watch(const string& path) {
    logger.log("Adding watch for path: " + path);

    // Add a fanotify mark on the specified path
    int ret = fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_CLOSE_WRITE, AT_FDCWD, path.c_str());
    if (ret == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    } else {
        logger.log("Watching " + path);
    }
}

bool EventDetector::is_hidden_path(const string& path) {
    stringstream ss(path);
    string segment;

    while (getline(ss, segment, '/')) {
        if (segment.length() > 0 && segment[0] == '.') {
            return true;
        }
    }
    return false;
}

bool EventDetector::is_whitelist_path(const string &path) {
    
    size_t pos = path.rfind('/');
    string path_without_file = path;
    if (pos != string::npos) {
        path_without_file = path_without_file.substr(0, pos + 1);
    }
    
    for(const auto& prefix : whitelist_dirs) {
        if(path.find(prefix) == 0) {
            return true;
        }
    }
    return false;
}

void EventDetector::process_events(ThreadPool& pool, atomic<bool>& running) {
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    char last_path[PATH_MAX];
    ssize_t path_len;
    char procfd_path[PATH_MAX];
    const char *str;

    while (running.load(memory_order_relaxed)) {
        len = read(fanotify_fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (len <= 0)
            break;

        metadata = buf;

        while (FAN_EVENT_OK(metadata, len) && running.load(memory_order_relaxed)) {
            if (metadata->vers != FANOTIFY_METADATA_VERSION) {
                fprintf(stderr, "Mismatch of fanotify metadata version.\n");
                exit(EXIT_FAILURE);
            }

            if (metadata->fd >= 0) {
                sprintf(procfd_path, "/proc/self/fd/%d", metadata->fd);
                path_len = readlink(procfd_path, path, sizeof(path) - 1);
                if (path_len == -1) {
                    perror("readlink");
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                string full_path(path);

                // Check if this is the same event as the last one
                if (strcmp(last_path, full_path.c_str()) == 0) {
                    close(metadata->fd);
                    metadata = FAN_EVENT_NEXT(metadata, len);
                    continue;
                }

                if (is_hidden_path(full_path) || is_whitelist_path(full_path)) {
                    close(metadata->fd);
                    metadata = FAN_EVENT_NEXT(metadata, len);
                    continue;
                }

                switch (metadata->mask) {
                    case FAN_ATTRIB:
                        str = "ATTRIB";
                        break;
                    case FAN_CREATE:
                        str = "CREATE";
                        break;
                    case FAN_DELETE:
                        str = "DELETE";
                        break;
                    case FAN_DELETE_SELF:
                        str = "DELETE SELF";
                        break;
                    case FAN_MOVED_FROM:
                        str = "MOVED FROM";
                        break;
                    case FAN_MOVED_TO:
                        str = "MOVED TO";
                        break;
                    case FAN_MOVE_SELF:
                        str = "MOVE SELF";
                        break;
                    case FAN_CLOSE_WRITE:
                        str = "FAN_CLOSE_WRITE";
                        break;
                    default:
                        str = "?";
                        break;
                }

                size_t last_slash = full_path.find_last_of("/");
                string filename = (last_slash != string::npos) ? full_path.substr(last_slash + 1) : full_path;

                time_t timestamp = time(nullptr);

                Event event(str, full_path, filename, timestamp, metadata->pid);

                strcpy(last_path, full_path.c_str());

                // Send event to thread pool
                pool.add_event(event);

                close(metadata->fd);
            }

            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }
}

int EventDetector::get_fanotify_fd() const {
    logger.log("Returning fanotify_fd: " + to_string(fanotify_fd));
    return fanotify_fd;
}
