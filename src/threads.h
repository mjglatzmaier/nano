#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <atomic>
#include <cassert>
#include <memory>
#include <tuple>

// #include "material.h"
// #include "pawns.h"

class WorkerThread {
private:
    std::thread thread_;

public:
    WorkerThread() = default;

    explicit WorkerThread(std::function<void()> func)
        : thread_(std::move(func)) {}

    WorkerThread(const WorkerThread &) = delete;
    WorkerThread &operator=(const WorkerThread &) = delete;

    WorkerThread(WorkerThread &&other) noexcept : thread_(std::move(other.thread_)) {}

    ~WorkerThread() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    std::thread &thread() { return thread_; }
};

class TimerThread : public WorkerThread {
public:
    TimerThread() {}
    TimerThread(std::function<void()> func) : WorkerThread(func) {}
};

template <class T>
class ThreadPool {
private:
    std::vector<std::unique_ptr<T>> workers_;
    std::queue<std::function<void()>> task_queue_;
    std::mutex mutex_;
    std::condition_variable cv_task_;
    std::condition_variable cv_finished_;
    std::atomic_uint busy_;
    std::atomic_uint processed_;
    std::atomic_bool stop_;
    unsigned int num_threads_;

    void thread_func() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_task_.wait(lock, [this]() { return stop_ || !task_queue_.empty(); });

            if (!task_queue_.empty()) {
                ++busy_;
                auto fn = std::move(task_queue_.front());
                task_queue_.pop();
                lock.unlock();
                fn();
                ++processed_;
                lock.lock();
                --busy_;
                cv_finished_.notify_one();
            } else if (stop_) {
                break;
            }
        }
    }

public:
    ThreadPool() {}

    explicit ThreadPool(const unsigned int num_threads) 
        : busy_(0), processed_(0), stop_(false), num_threads_(num_threads) {
        for (unsigned int i = 0; i < num_threads_; ++i) {
            workers_.emplace_back(std::make_unique<T>([this] { thread_func(); }));
        }
    }

    ~ThreadPool() {
        if (!stop_) {
            exit();
        }
    }

    void init(const int num_threads) {
        exit();
        workers_.clear();
        busy_ = 0;
        processed_ = 0;
        stop_ = false;
        num_threads_ = num_threads;

        for (unsigned int i = 0; i < num_threads_; ++i) {
            workers_.emplace_back(std::make_unique<T>([this] { thread_func(); }));
        }
    }

    template<class TT, typename... Args>
    void enqueue(TT&& f, Args&&... args) {
        std::unique_lock<std::mutex> lock(mutex_);
        task_queue_.emplace([func = std::forward<TT>(f), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            std::apply(func, args_tuple);
        });
        cv_task_.notify_one();
    }

    void wait_finished() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_finished_.wait_for(lock, std::chrono::seconds(5), [this]() {
            return task_queue_.empty() && (busy_ == 0);
        });
    }

    void exit() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_task_.notify_all();
        wait_finished();

        for (auto& t : workers_) {
            if (t->thread().joinable()) {
                t->thread().join();
            }
        }
        workers_.clear();
    }

    size_t num_workers() const { return workers_.size(); }

    unsigned int size() const { return num_threads_; }

    unsigned int get_processed() const { return processed_; }

    void clear_tasks() {
        while (!task_queue_.empty()) {
            task_queue_.pop();
        }
    }
};

#endif
