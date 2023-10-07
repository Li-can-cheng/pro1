#include "pool.h"

template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;

public:
    ThreadSafeQueue() {}

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

ThreadSafeQueue<Task> tasks;

void worker() {
    while (true) {
        Task task;
        if (tasks.try_pop(task)) {
            task.handler(task.req, task.res);
        }
        // 这里可以加入一个小的延时，避免线程过度运行
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
