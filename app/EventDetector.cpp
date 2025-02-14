#include "EventDetector.h"

// Constructor for EventDetector
EventDetector::EventDetector() {
    std::cout << "Initializing EventDetector..." << std::endl;
    // Initialize fanotify file descriptor
    fanotify_fd = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
    std::cout << "fanotify_fd initialized with FD: " << fanotify_fd << std::endl;
}

// Destructor for EventDetector
EventDetector::~EventDetector() {
    std::cout << "Closing fanotify_fd: " << fanotify_fd << std::endl;
    // Close the fanotify file descriptor
    close(fanotify_fd);
}

// Add a watch on the specified path
void EventDetector::add_watch(const std::string& path) {
    std::cout << "Adding watch for path: " << path << std::endl;

    // Add a fanotify mark on the specified path
    ret = fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_ONDIR | FAN_EVENT_ON_CHILD | FAN_CLOSE_WRITE, AT_FDCWD, path.c_str());
    if (ret == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Watching " << path << std::endl;
    }
}

// Process events from the fanotify file descriptor
void EventDetector::process_events() {
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    ssize_t path_len;
    char procfd_path[PATH_MAX];
    struct fanotify_response response;
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

            if (metadata->fd >= 0) {
                // Handle permission events
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

                // Get the path of the file associated with the event
                sprintf(procfd_path, "/proc/self/fd/%d", metadata->fd);
                path_len = readlink(procfd_path, path, sizeof(path) - 1);
                if (path_len == -1) {
                    perror("readlink");
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                printf("File %s\n", path);

                std::string full_path(path);

                // Extract filename and extension from the full path
                size_t last_slash = full_path.find_last_of("/");
                std::string filename = (last_slash != std::string::npos) ? full_path.substr(last_slash + 1) : full_path;
                size_t dot_pos = filename.find_last_of(".");
                std::string extension = (dot_pos != std::string::npos) ? filename.substr(dot_pos + 1) : "";

                // Get the current timestamp
                std::time_t timestamp = std::time(nullptr);

                // Create an Event object
                Event event(str, full_path, filename, extension, timestamp);

                // Log the event
                log_event(event);

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
    std::cout << "Enqueuing event: " << event.get_event_type() << " - " << event.get_filepath() << std::endl;
    if (event_queue.size() >= max_queue_size) {
        std::cout << "Event queue is full, popping an event." << std::endl;
        event_queue.pop();
    }
    event_queue.push(event);
}

// Get the fanotify file descriptor
int EventDetector::get_fanotify_fd() const {
    std::cout << "Returning fanotify_fd: " << fanotify_fd << std::endl;
    return fanotify_fd;
}
