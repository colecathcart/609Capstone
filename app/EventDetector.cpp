#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <sys/fanotify.h>
#include <unistd.h>
#include <linux/limits.h>

struct Event {
    std::string event_type;
    std::string filepath;
    std::string filename;
    std::string extension;
    std::time_t timestamp;
};

class EventDetector {
    private:
        int fanotify_fd;
        std::queue<Event> event_queue;
        size_t max_queue_size = 100;

    public:
        EventDetector() {
            fanotify_fd = fantify_init(FAN_CLASS_CONTENT | FAN_CLOEEXEC | FAN_NONBLOCK, O_RDONLY);
            if (fanotify_fd == -1) {
                perror("fanotify_init");
                exit(EXIT_FAILURE);
            }
        }

        ~EventDetector() {
            close(fanotify_fd);
        }

        void add_watch(const std::string& path) {
            if (fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_CREATE | FAN_DELETE | FAN_MODIFY, AT_FDCWD, path.c_str()) == -1) {
                perror("fanotify_mark");
                exit(EXIT_FAILURE);
            }
        }

        void process_events() {
            
        }