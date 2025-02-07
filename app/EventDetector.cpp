#include "EventDetector.h"

EventDetector::EventDetector() {
    fanotify_fd = fanotify_init(FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
}

EventDetector::~EventDetector() {
    close(fanotify_fd);
}

void EventDetector::add_watch(const std::string& path) {
    if (fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_OPEN | FAN_CLOSE_WRITE | FAN_EVENT_ON_CHILD, AT_FDCWD, path.c_str()) == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    }
}

void EventDetector::process_events() {
    char buffer[4096];
    ssize_t length;

    while ((length = read(fanotify_fd, buffer, sizeof(buffer))) > 0) {
        struct fanotify_event_metadata *metadata = (struct fanotify_event_metadata *)buffer;
        while (FAN_EVENT_OK(metadata, length)) {
            if (metadata->vers != FANOTIFY_METADATA_VERSION) {
                std::cerr << "Invalid fanotify metadata version\n";
                exit(EXIT_FAILURE);
            }

            if (metadata->fd >= 0) {
                char filepath[PATH_MAX];
                sprintf(filepath, "/proc/self/fd/%d", metadata->fd);
                char resolved_path[PATH_MAX];
                ssize_t path_length = readlink(filepath, resolved_path, PATH_MAX);
                resolved_path[path_length] = '\0';
                close(metadata->fd);

                std::string event_type;
                if (metadata->mask & FAN_CREATE) event_type = "write";    
                else if (metadata->mask & FAN_DELETE) event_type = "delete";
                else continue;

                std::string path_str(resolved_path);
                size_t last_slash = path_str.find_last_of("/");
                std::string filename = path_str.substr(last_slash + 1);
                std::string extension = filename.substr(filename.find_last_of(".") + 1);

                Event event = {event_type, path_str, filename, extension, std::time(nullptr)};
                log_event(event);
                enqueue_event(event);
            }
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

int main() {
    EventDetector detector;
    detector.add_watch("/tmp");
    detector.process_events();
    return 0;
}
