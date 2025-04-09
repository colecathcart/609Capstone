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

/**
 * @brief Organizes a pool of waiting threads to perform analysis jobs on
 * incoming events from the EventDetector.
 */
class ThreadPool
{

public:
    /**
     * @brief Constructor
     * @param num_threads The number of threads to add to the pool
     */
    ThreadPool(size_t num_threads);
    /**
     * @brief Destructor
     */
    ~ThreadPool();
    /**
     * @brief Adds an event to the job queue
     */
    void add_event(Event& event);

private:
    /**
     * @brief Shared thread-safe analyzer instance for processing events in the queue
     */
    Analyzer analyzer;
    /**
     * @brief Vector of all threads in the pool
     */
    vector<thread> workers;
    /**
     * @brief Queue of event jobs, filled by the EventDetector
     */
    queue<Event> event_queue;
    /**
     * @brief Mutex for queue thread safety
     */
    mutex queue_mutex;
    /**
     * @brief Condition variable for waking threads
     */
    condition_variable cv;
    /**
     * @brief Atomic switch to shut down threads
     */
    atomic<bool> stop;
    /**
     * @brief Main waiting loop for worker threads
     */
    void worker();
    
};

#endif