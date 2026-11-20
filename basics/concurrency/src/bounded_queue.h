// bounded_queue.h - a thread-safe queue with a capacity limit.
// push() waits while the queue is full; pop() waits while it is empty.
// close() wakes everyone: pop() then drains what is left and returns
// std::nullopt, and push() returns false.
#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <utility>

template <class T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

    bool push(T value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // wait() unlocks, sleeps and relocks; the predicate guards against
        // spurious wakeups and against conditions that changed before we woke.
        not_full_.wait(lock, [this] { return items_.size() < capacity_ || closed_; });
        if (closed_)
            return false;
        items_.push_back(std::move(value));
        lock.unlock();                 // notify after unlocking: the woken thread can run at once
        not_empty_.notify_one();
        return true;
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return !items_.empty() || closed_; });
        if (items_.empty())
            return std::nullopt;       // closed and drained
        T value = std::move(items_.front());
        items_.pop_front();
        lock.unlock();
        not_full_.notify_one();
        return value;
    }

    void close()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::deque<T> items_;
    const std::size_t capacity_;
    bool closed_ = false;
};
