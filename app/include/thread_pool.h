#include <queue>
#include <atomic>
#include <condition_variable>
#include "event.h"
#include <thread>
#include <vector>
#include <mutex>
#include "analyzer.h"
#ifndef THREADPOOL
#define THREADPOOL

using namespace std;

class ThreadPool
{

public:
    ThreadPool(size_t num_threads);

    ~ThreadPool();

    void add_event(Event& event);

private:
    Analyzer analyzer;
    vector<thread> workers;
    queue<Event> event_queue;
    mutex queue_mutex;
    condition_variable cv;
    atomic<bool> stop;

    void worker();
    
};

#endif