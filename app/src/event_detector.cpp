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

using namespace std;

// Constructor for EventDetector
EventDetector::EventDetector(): analyzer(Analyzer()) {
    cout << "Initializing EventDetector..." << endl;
    // Initialize fanotify file descriptor
    fanotify_fd = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
    cout << "fanotify_fd initialized with FD: " << fanotify_fd << endl;
}

// Destructor for EventDetector
EventDetector::~EventDetector() {
    cout << "Closing fanotify_fd: " << fanotify_fd << endl;
    // Close the fanotify file descriptor
    close(fanotify_fd);
}

// Add a watch on the specified path
void EventDetector::add_watch(const string& path) {
    cout << "Adding watch for path: " << path << endl;

    // Add a fanotify mark on the specified path
    int ret = fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_CLOSE_WRITE, AT_FDCWD, path.c_str());
    if (ret == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    } else {
        cout << "Watching " << path << endl;
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

bool EventDetector::is_no_concern_path(const std::string &path) {
    // Check if the path contains /tmp/ or /var/spool/
    if (path.find("/tmp/") != string::npos || path.find("/var/spool/") != string::npos) {
        return true;
    }
    return false;
}

// Process events from the fanotify file descriptor
void EventDetector::process_events() {
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    char last_path[PATH_MAX];
    ssize_t path_len;
    char procfd_path[PATH_MAX];
    // struct fanotify_response response;
    const char *str;

    for (;;) {
        // Read events from the fanotify file descriptor
        len = read(fanotify_fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (len <= 0)
            break;

        metadata = buf;

        // Process each event
        while (FAN_EVENT_OK(metadata, len)) {
            if (metadata->vers != FANOTIFY_METADATA_VERSION) {
                fprintf(stderr, "Mismatch of fanotify metadata version.\n");
                exit(EXIT_FAILURE);
            }

            if (metadata->fd >= 0) {
                // Get the path of the file associated with the event
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

                // **Skip hidden files and directories BEFORE checking event type**
                if (is_hidden_path(full_path) || is_no_concern_path(full_path)) {
                    close(metadata->fd);
                    metadata = FAN_EVENT_NEXT(metadata, len);
                    continue;
                }

                // Determine the type of event
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

                // Can likely remove this code block in final version
                /* Handle permission events
                if (metadata->mask & FAN_OPEN_PERM) {
                    printf("FAN_OPEN_PERM: ");

                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write(fanotify_fd, &response, sizeof(response));
                }

                // Handle close write events
                if (metadata->mask & FAN_CLOSE_WRITE) {
                    printf("FAN_CLOSE_WRITE: ");
                }

                printf("File %s\n", path); */

                // Extract filename and extension from the full path
                size_t last_slash = full_path.find_last_of("/");
                string filename = (last_slash != string::npos) ? full_path.substr(last_slash + 1) : full_path;
                size_t dot_pos = filename.find_last_of(".");
                string extension = (dot_pos != string::npos) ? filename.substr(dot_pos + 1) : "";

                // Get the current timestamp
                time_t timestamp = time(nullptr);

                // Create an Event object
                Event event(str, full_path, filename, extension, timestamp, metadata->pid);

                // Log the event
                log_event(event);

                // Save last path
                strcpy(last_path, full_path.c_str());

                // Call analyzer

                analyzer.analyze(event);

                // Close the file descriptor associated with the event
                close(metadata->fd);
            }

            // Move to the next event
            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }
}

// Log the event details
void EventDetector::log_event(const Event& event) {
    event.print();
}

// Enqueue the event into the event queue
void EventDetector::enqueue_event(const Event& event) {
    cout << "Enqueuing event: " << event.get_event_type() << " - " << event.get_filepath() << endl;
    if (event_queue.size() >= max_queue_size) {
        cout << "Event queue is full, popping an event." << endl;
        event_queue.pop();
    }
    event_queue.push(event);
}

// Get the fanotify file descriptor
int EventDetector::get_fanotify_fd() const {
    cout << "Returning fanotify_fd: " << fanotify_fd << endl;
    return fanotify_fd;
}
