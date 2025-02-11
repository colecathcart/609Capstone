#include "EventDetector.h"

EventDetector::EventDetector() {
    std::cout << "Initializing EventDetector..." << std::endl;
    fanotify_fd = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME, 0);
    if (fanotify_fd == -1) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }
    std::cout << "fanotify_fd initialized with FD: " << fanotify_fd << std::endl;
}

EventDetector::~EventDetector() {
    std::cout << "Closing fanotify_fd: " << fanotify_fd << std::endl;
    close(fanotify_fd);
}

void EventDetector::add_watch(const std::string& path) {
    std::cout << "Adding watch for path: " << path << std::endl;
    mount_fd = open(path.c_str(), O_DIRECTORY | O_RDONLY);
    if (mount_fd == -1) {
        perror(path.c_str());
        exit(EXIT_FAILURE);
    }

    ret = fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_ONLYDIR, FAN_CREATE | FAN_DELETE | FAN_EVENT_ON_CHILD | FAN_CLOSE_WRITE | FAN_ONDIR, AT_FDCWD, path.c_str());
    if (ret == -1) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Watching " << path << std::endl;
    }
}

void EventDetector::process_events() {
    int event_fd;
    ssize_t size, path_len;
    char path[PATH_MAX];
    char procfd_path[PATH_MAX];
    char events_buf[4096];
    struct file_handle *file_handle;
    struct fanotify_event_metadata *metadata;
    struct fanotify_event_info_fid *fid;
    const char *file_name;
    struct stat sb;

    /* Read events from the event queue into a buffer. */

    size = read(fanotify_fd, events_buf, sizeof(events_buf));
    if (size == -1 && errno != EAGAIN) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* Process all events within the buffer. */

    for (metadata = (struct fanotify_event_metadata *) events_buf;
            FAN_EVENT_OK(metadata, size);
            metadata = FAN_EVENT_NEXT(metadata, size)) {
        fid = (struct fanotify_event_info_fid *) (metadata + 1);
        file_handle = (struct file_handle *) fid->handle;

        /* Ensure that the event info is of the correct type. */

        if (fid->hdr.info_type == FAN_EVENT_INFO_TYPE_FID ||
            fid->hdr.info_type == FAN_EVENT_INFO_TYPE_DFID) {
            file_name = NULL;
        } else if (fid->hdr.info_type == FAN_EVENT_INFO_TYPE_DFID_NAME) {
            file_name = (const char *)(file_handle->f_handle +
                        file_handle->handle_bytes);
        } else {
            fprintf(stderr, "Received unexpected event info type.\n");
            exit(EXIT_FAILURE);
        }

        if (metadata->mask == FAN_CREATE)
            printf("FAN_CREATE (file created):\n");

        if (metadata->mask == (FAN_CREATE | FAN_ONDIR))
            printf("FAN_CREATE | FAN_ONDIR (subdirectory created):\n");

    /* metadata->fd is set to FAN_NOFD when the group identifies
        objects by file handles.  To obtain a file descriptor for
        the file object corresponding to an event you can use the
        struct file_handle that's provided within the
        fanotify_event_info_fid in conjunction with the
        open_by_handle_at(2) system call.  A check for ESTALE is
        done to accommodate for the situation where the file handle
        for the object was deleted prior to this system call. */

        event_fd = open_by_handle_at(mount_fd, file_handle, O_RDONLY);
        if (event_fd == -1) {
            if (errno == ESTALE) {
                printf("File handle is no longer valid. "
                        "File has been deleted\n");
                continue;
            } else {
                perror("open_by_handle_at");
                exit(EXIT_FAILURE);
            }
        }

        snprintf(procfd_path, sizeof(procfd_path), "/proc/self/fd/%d",
                event_fd);

        /* Retrieve and print the path of the modified dentry. */

        path_len = readlink(procfd_path, path, sizeof(path) - 1);
        if (path_len == -1) {
            perror("readlink");
            exit(EXIT_FAILURE);
        }

        path[path_len] = '\0';
        printf("\tDirectory '%s' has been modified.\n", path);

        if (file_name) {
            ret = fstatat(event_fd, file_name, &sb, 0);
            if (ret == -1) {
                if (errno != ENOENT) {
                    perror("fstatat");
                    exit(EXIT_FAILURE);
                }
                printf("\tEntry '%s' does not exist.\n", file_name);
            } else if ((sb.st_mode & S_IFMT) == S_IFDIR) {
                printf("\tEntry '%s' is a subdirectory.\n", file_name);
            } else {
                printf("\tEntry '%s' is not a subdirectory.\n",
                        file_name);
            }
        }

        /* Close associated file descriptor for this event. */

        close(event_fd);
    }



}

void EventDetector::log_event(const Event& event) {
    std::cout << "[" << std::ctime(&event.time) << "] " << event.event_type << " - " << event.filepath << " (" << event.filename << ")" << std::endl;
}

void EventDetector::enqueue_event(const Event& event) {
    std::cout << "Enqueuing event: " << event.event_type << " - " << event.filepath << std::endl;
    if (event_queue.size() >= max_queue_size) {
        std::cout << "Event queue is full, popping an event." << std::endl;
        event_queue.pop();
    }
    event_queue.push(event);
}

int EventDetector::get_fanotify_fd() const {
    std::cout << "Returning fanotify_fd: " << fanotify_fd << std::endl;
    return fanotify_fd;
}
