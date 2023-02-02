#pragma once

#include "CoroutineTask.hpp"
#include "FunctionTask.hpp"
#include "Pool.hpp"
#include "Queue.hpp"
#include "Task.hpp"

#include <any>
#include <atomic>
#include <condition_variable>
#include <coroutine>
#include <functional>

namespace IE::Core::Threading {
class ThreadPool;

struct ResumeAfter {
    ResumeAfter() = default;

    template<typename... Args>
    ResumeAfter(ThreadPool *t_threadPool, Args... args) :
            m_ready([args...] { return (... && args->finished()); }),
            m_threadPool(t_threadPool) {
    }

    ResumeAfter(ThreadPool *t_threadPool, std::vector<std::shared_ptr<BaseTask>> t_tasks) :
            m_ready([t_tasks] {
                return std::all_of(t_tasks.begin(), t_tasks.end(), [](std::shared_ptr<BaseTask> task) {
                    return task->finished();
                });
            }),
            m_threadPool(t_threadPool) {
    }

    // Indicates the readiness of the coroutine to continue. True -> resume, False -> suspend
    bool await_ready() {
        return m_ready();
    }

    void await_suspend(std::coroutine_handle<> t_handle);

    void await_resume() {
    }

    void resume() {
        m_handle.resume();
    }

private:
    ThreadPool             *m_threadPool;
    std::function<bool()>   m_ready;
    std::coroutine_handle<> m_handle;
};

class Worker {
public:
    Worker(ThreadPool *t_threadPool);
};

class ThreadPool {
    std::vector<std::thread>         m_workers;
    Queue<std::shared_ptr<BaseTask>> m_activeQueue;
    Pool<ResumeAfter>                m_suspendedPool;
    std::condition_variable          m_workAssignmentConditionVariable;
    std::atomic<bool>                m_shutdown{false};

public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) {
        m_workers.reserve(threads);
        for (; threads > 0; --threads) m_workers.emplace_back([this] { Worker(this); });
    }

    template<typename T>
    auto submit(CoroutineTask<T> &&f) -> std::shared_ptr<Task<T>> {
        auto job{std::make_shared<CoroutineTask<T>>(f)};
        job->connectHandle();
        m_activeQueue.push(std::static_pointer_cast<BaseTask>(job));
        m_workAssignmentConditionVariable.notify_one();
        return std::static_pointer_cast<Task<T>>(job);
    }

    template<typename T, typename... Args>

        requires requires(T &&f, Args &&...args) { typename decltype(f(args...))::ReturnType; }

    auto submit(T &&f, Args &&...args) -> std::shared_ptr<Task<typename decltype(f(args...))::ReturnType>> {
        auto job{std::make_shared<CoroutineTask<typename decltype(f(args...))::ReturnType>>(f(args...))};
        job->connectHandle();
        m_activeQueue.push(std::static_pointer_cast<BaseTask>(job));
        m_workAssignmentConditionVariable.notify_one();
        return job;
    }

    template<typename T, typename... Args>
    auto submit(T &&f, Args &&...args) -> std::shared_ptr<Task<decltype(f(args...))>> {
        auto job{std::make_shared<FunctionTask<decltype(f(args...))>>([f, ... args = std::forward<Args>(args)] {
            return f(args...);
        })};
        m_activeQueue.push(std::static_pointer_cast<BaseTask>(job));
        m_workAssignmentConditionVariable.notify_one();
        return job;
    }

    template<typename... Args>
    ResumeAfter resumeAfter(Args... args) {
        return {this, args...};
    }

    ResumeAfter resumeAfter(std::vector<std::shared_ptr<BaseTask>> t_tasks) {
        return {this, t_tasks};
    }

    ~ThreadPool() {
        m_shutdown = true;
        m_workAssignmentConditionVariable.notify_all();
        for (std::thread &thread : m_workers)
            if (thread.joinable()) thread.join();
    }

    uint32_t getWorkerCount() {
        return m_workers.size();
    }

    friend Worker::Worker(ThreadPool *t_threadPool);
    friend void ResumeAfter::await_suspend(std::coroutine_handle<> t_handle);
};
}  // namespace IE::Core::Threading