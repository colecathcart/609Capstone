#include "EventDetector.h"

EventDetector::EventDetector() {
    fanotify_fd = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_FID, O_RDWR);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
}

EventDetector::~EventDetector() {
    close(fanotify_fd);
}

void EventDetector::add_watch(const std::string& path) {
    if (fanotify_mark(fanotify_fd, FAN_MARK_ADD, FAN_CREATE | FAN_DELETE | FAN_EVENT_ON_CHILD | FAN_CLOSE_WRITE, AT_FDCWD, path.c_str()) == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Watching " << path << std::endl;
        }
}

void EventDetector::process_events() {
    char buffer[4096];
    ssize_t length = read(fanotify_fd, buffer, sizeof(buffer));
    if (length > 0) {
        struct fanotify_event_metadata *metadata = (struct fanotify_event_metadata *)buffer;
        while (FAN_EVENT_OK(metadata, length)) {
            Event event;
            event.time = std::time(nullptr);

            if (metadata->mask & FAN_CREATE) {
                event.event_type = "create";
            } else if (metadata->mask & FAN_DELETE) {
                event.event_type = "delete";
            } else if (metadata->mask & FAN_CLOSE_WRITE) {
                event.event_type = "write";
            } else {
                event.event_type = "unknown";
            }

            // Handle FAN_REPORT_FID case
            if (metadata->fd == -1 && (metadata->vers >= 3)) {
                struct fanotify_event_info_fid *fid_info =
                    (struct fanotify_event_info_fid *)(metadata + 1);
                struct file_handle *file_handle = (struct file_handle *)fid_info->handle;

                // Retrieve the file path using name_to_handle_at or open_by_handle_at
                char filepath[PATH_MAX];
                int dir_fd = open("/", O_PATH); // Root directory for lookup
                int file_fd = open_by_handle_at(dir_fd, file_handle, O_RDONLY);
                if (file_fd != -1) {
                    ssize_t len = readlink(("/proc/self/fd/" + std::to_string(file_fd)).c_str(), filepath, PATH_MAX);
                    if (len != -1) {
                        filepath[len] = '\0';
                        event.filepath = filepath;
                    }
                    close(file_fd);
                }
                close(dir_fd);
            } else {
                // Handle normal file descriptor case
                char filepath[PATH_MAX];
                snprintf(filepath, PATH_MAX, "/proc/self/fd/%d", metadata->fd);
                ssize_t len = readlink(filepath, filepath, PATH_MAX);
                if (len != -1) {
                    filepath[len] = '\0';
                    event.filepath = filepath;
                }
                close(metadata->fd);
            }

            // Extract filename and extension
            std::string filename = event.filepath;
            size_t pos = filename.find_last_of("/");
            event.filename = (pos != std::string::npos) ? filename.substr(pos + 1) : filename;
            size_t ext_pos = event.filename.find_last_of(".");
            event.extension = (ext_pos != std::string::npos) ? event.filename.substr(ext_pos + 1) : "";

            enqueue_event(event);
            log_event(event);

            metadata = FAN_EVENT_NEXT(metadata, length);
        }
    }
}



void EventDetector::log_event(const Event& event) {
    std::cout << "[" << std::ctime(&event.time) << "] " << event.event_type << " - " << event.filepath << " (" << event.filename << ")" << std::endl;
}

void EventDetector::enqueue_event(const Event& event) {
    if (event_queue.size() >= max_queue_size) {
        event_queue.pop();
    }
    event_queue.push(event);
}

int EventDetector::get_fanotify_fd() const {
    return fanotify_fd;
}