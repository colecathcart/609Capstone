#include <thread_pool.h>

using namespace std;

ThreadPool::ThreadPool(size_t num_threads) : analyzer(Analyzer()), stop(false) {
    for (size_t i = 0; i < num_threads; i++) {
            workers.push_back(thread([this] {
                this->worker();
        }));
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (auto& worker: workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::add_event(Event& event) {
    {
        lock_guard<mutex> lock(queue_mutex);
        event_queue.push(event);
    }
    cv.notify_one();
}

void ThreadPool::worker() {
    while (!stop) {
        Event event;
        {
            unique_lock<mutex> lock(queue_mutex);
            cv.wait(lock, [this] {return !event_queue.empty() || stop;});

            if (stop) {
                return;
            }

            event = event_queue.front();
            event_queue.pop();
        }
        analyzer.analyze(event);
    }
}