#pragma once

#include "Core/ThreadingModule/Awaitable.hpp"
#include "CoroutineTask.hpp"
#include "EnsureThread.hpp"
#include "FunctionTask.hpp"
#include "Queue.hpp"
#include "ResumeAfter.hpp"
#include "ResumeOnMainThreadAfter.hpp"
#include "Task.hpp"
#include "Worker.hpp"

#if defined(AppleClang)
#    include <experimental/coroutine>

namespace std {
using std::experimental::coroutine_handle;
using std::experimental::suspend_always;
using std::experimental::suspend_never;
}  // namespace std
#else
#    include <coroutine>
#endif
#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>

/**@todo Enable waiting on non-thread pool related things. (e.g. sleep(1))*/
namespace IE::Core::Threading {
class ThreadPool {
    std::vector<std::thread>         m_workers;
    Queue<std::shared_ptr<BaseTask>> m_queue;
    Queue<std::shared_ptr<BaseTask>> m_mainQueue;
    std::condition_variable_any      m_workAssignedNotifier;
    std::condition_variable_any      m_mainWorkAssignedNotifier;
    std::atomic<bool>                m_mainShutdown{false};
    std::thread::id                  mainThreadID;
    std::atomic<uint32_t>            m_threadShutdownCount{0};

public:
    explicit ThreadPool(uint32_t t_threads = std::thread::hardware_concurrency());

    void startMainThreadLoop();

    template<typename T>
    auto submit(CoroutineTask<T> &&t_coroutine) -> std::shared_ptr<Task<T>> {
        auto task{std::make_shared<CoroutineTask<T>>(t_coroutine)};
        task->connectHandle();
        m_queue.push(std::static_pointer_cast<BaseTask>(task));
        m_workAssignedNotifier.notify_one();
        return std::static_pointer_cast<Task<T>>(task);
    }

    template<typename T, typename... Args>
    auto submit(T &&t_function, Args &&...args) -> std::shared_ptr<Task<decltype(t_function(args...))>> {
        auto task{std::make_shared<CoroutineTask<decltype(t_function(args...))>>(
          [&t_function, &args...]->CoroutineTask<decltype(t_function(args...))> {co_return t_function(args...);}()
        )};
        task->connectHandle();
        m_queue.push(std::static_pointer_cast<BaseTask>(task));
        m_workAssignedNotifier.notify_one();
        return task;
    }

    template<typename T>
    auto submitToMainThread(CoroutineTask<T> &&t_coroutine) -> std::shared_ptr<Task<T>> {
        auto task{std::make_shared<CoroutineTask<T>>(t_coroutine)};
        task->connectHandle();
        m_mainQueue.push(std::static_pointer_cast<BaseTask>(task));
        m_mainWorkAssignedNotifier.notify_one();
        return std::static_pointer_cast<Task<T>>(task);
    }

    template<typename T, typename... Args>
    auto submitToMainThread(T &&t_function, Args &&...args)
      -> std::shared_ptr<Task<decltype(t_function(args...))>> {
        auto task{std::make_shared<CoroutineTask<decltype(t_function(args...))>>(
          [t_function, args...]->CoroutineTask<decltype(t_function(args...))> {co_return t_function(args...);}()
        )};
        task->connectHandle();
        m_mainQueue.push(std::static_pointer_cast<BaseTask>(task));
        m_mainWorkAssignedNotifier.notify_one();
        return task;
    }

    template<typename T>
    T executeInPlace(CoroutineTask<T> &&t_coroutine) {
        auto task{std::make_shared<CoroutineTask<T>>(t_coroutine)};
        task->connectHandle();
        m_queue.push(std::static_pointer_cast<BaseTask>(task));
        m_workAssignedNotifier.notify_one();
        Worker::loopUntilTaskFinished(this, &t_coroutine);
        return t_coroutine.value();
    }

    template<typename... Args>
    ResumeAfter resumeAfter(Args... args) {
        return ResumeAfter{this, args...};
    }

    template<typename... Args>
    ResumeOnMainThreadAfter resumeOnMainThreadAfter(Args... args) {
        return ResumeOnMainThreadAfter{this, args...};
    }

    EnsureThread ensureThread(ThreadType t_type) {
        return {this, t_type};
    }

    ~ThreadPool();

    uint32_t getWorkerCount();

    void shutdown();

    void setWorkerCount(uint32_t t_threads = std::thread::hardware_concurrency());

    friend void Worker::start(IE::Core::Threading::ThreadPool *t_threadPool);
    friend void Worker::loopUntilTaskFinished(ThreadPool *t_threadPool, BaseTask *t_stopTask);
    friend bool EnsureThread::await_ready();
};
}  // namespace IE::Core::Threading