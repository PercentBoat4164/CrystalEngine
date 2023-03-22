#include "Worker.hpp"

#include "Task.hpp"
#include "ThreadPool.hpp"

#include <memory>
#include <mutex>

void IE::Core::Threading::Worker::start(ThreadPool *t_threadPool) {
    ThreadPool               &pool = *t_threadPool;
    std::shared_ptr<BaseTask> activeJob;
    std::mutex                mutex;
    while (!pool.m_shutdown) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!pool.m_activeQueue.pop(activeJob))
            pool.m_workAssignmentConditionVariable.wait(lock, [&] {
                return pool.m_activeQueue.pop(activeJob) || pool.m_shutdown;
            });
        if (pool.m_shutdown) break;
        activeJob->execute();
    }
}
