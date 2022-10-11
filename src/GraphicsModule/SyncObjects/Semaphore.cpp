#include "Semaphore.hpp"

#include "RenderEngine.hpp"

IE::Graphics::Semaphore::Semaphore(std::weak_ptr<IE::Graphics::RenderEngine> t_engineLink) {
    create(t_engineLink);
}

std::future<void> IE::Graphics::Semaphore::wait() {
    std::packaged_task<void()> wait{[&] {
        blocking_wait();
    }};
    IE::Core::Core::getInst().threadPool.submit([&] { wait(); });
    return wait.get_future();
}

void IE::Graphics::Semaphore::blocking_wait() {
    VkSemaphoreWaitInfo waitInfo{
      .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
      .pNext          = nullptr,
      .flags          = 0,
      .semaphoreCount = 1,
      .pSemaphores    = &semaphore,
      .pValues        = nullptr,
    };

    // The waiting process can happen on multiple threads at once, but destruction cannot happen while waiting is
    // happening. Therefore, this synchronization does not the waiting if the lock cannot be gotten, but does
    // ensure that the mutex is locked when waiting.
    bool locked{semaphoreMutex->try_lock()};
    linkedRenderEngine.lock()->getLogger().log("Waiting for Semaphore");
    status = IE_SEMAPHORE_STATUS_WAITING;
    vkWaitSemaphores(linkedRenderEngine.lock()->getDevice(), &waitInfo, std::numeric_limits<uint64_t>::max());
    status = IE_SEMAPHORE_STATUS_VALID;
    if (locked) semaphoreMutex->unlock();
}

IE::Graphics::Semaphore::~Semaphore() {
    std::unique_lock<std::mutex> lock(*semaphoreMutex);
    status = IE_SEMAPHORE_STATUS_INVALID;
    vkDestroySemaphore(linkedRenderEngine.lock()->getDevice(), semaphore, nullptr);
}

IE::Graphics::Semaphore::Semaphore(const IE::Graphics::Semaphore &t_other) {
    if (this != &t_other) {
        std::unique_lock<std::mutex> lock(*semaphoreMutex);
        std::unique_lock<std::mutex> otherLock(*t_other.semaphoreMutex);
        status.store(t_other.status.load());
        semaphore          = t_other.semaphore;
        linkedRenderEngine = t_other.linkedRenderEngine;
    }
}

void IE::Graphics::Semaphore::create(std::weak_ptr<IE::Graphics::RenderEngine> t_engineLink) {
    linkedRenderEngine = t_engineLink;
    semaphoreMutex = std::make_shared<std::mutex>();
    VkSemaphoreCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
    };
    vkCreateSemaphore(linkedRenderEngine.lock()->getDevice(), &createInfo, nullptr, &semaphore);
    status = IE_SEMAPHORE_STATUS_VALID;
    linkedRenderEngine.lock()->getLogger().log("Created Semaphore");
}
