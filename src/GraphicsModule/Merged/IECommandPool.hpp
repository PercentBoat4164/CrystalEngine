#pragma once

#include "IERenderEngineLink.hpp"
#include "Core/LogModule/IELogger.hpp"

#ifdef ILLUMINATION_ENGINE_VULKAN
#include "VkBootstrap.h"
#endif

class IECommandPool {
public:
    struct CreateInfo {
    public:
        #ifdef ILLUMINATION_ENGINE_VULKAN
        VkCommandPoolCreateFlagBits flags{static_cast<VkCommandPoolCreateFlagBits>(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)};    // Optional
        vkb::QueueType commandQueue{vkb::QueueType::graphics}; // Optional
        #endif
    };

    struct Created {
    public:
        bool commandPool{};
        bool commandBuffers{};
    };
    CreateInfo createdWith{};
    Created created{};
    #ifdef ILLUMINATION_ENGINE_VULKAN
    VkCommandPool commandPool{};
    std::vector<VkCommandBuffer> commandBuffers{};
    #endif
    IERenderEngineLink *linkedRenderEngine{};

    void create(IERenderEngineLink *engineLink, CreateInfo *createInfo) {
        linkedRenderEngine = engineLink;
        #ifdef ILLUMINATION_ENGINE_VULKAN
        VkCommandPoolCreateInfo commandPoolCreateInfo{.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .flags=createInfo->flags, .queueFamilyIndex=linkedRenderEngine->device.get_queue_index(createInfo->commandQueue).value()};
        if (vkCreateCommandPool(linkedRenderEngine->device.device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
            IELogger::logDefault(ILLUMINATION_ENGINE_LOG_LEVEL_DEBUG, "Failed to create command pool!");
        }
        created.commandPool = true;
        #endif
    }

    void addCommandBuffers(uint32_t commandBufferCount) {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        commandBuffers.resize(commandBufferCount + commandBuffers.size());
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .commandPool=commandPool, .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY, .commandBufferCount=static_cast<uint32_t>(commandBuffers.size())};
        if (vkAllocateCommandBuffers(linkedRenderEngine->device.device, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            IELogger::logDefault(ILLUMINATION_ENGINE_LOG_LEVEL_DEBUG, "Failed to allocate command buffers!");
        }
        created.commandBuffers = true;
        #endif
    };

    void resetCommandBuffer(const std::vector<uint32_t> &resetIndices) {
        for (uint32_t i : resetIndices) { this->resetCommandBuffer(i); }
    }

    void resetCommandBuffer(uint32_t resetIndex = 0) {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        VkCommandBufferResetFlags commandBufferResetFlags{};
        vkResetCommandBuffer(commandBuffers[resetIndex], commandBufferResetFlags);
        #endif
    }

    void recordCommandBuffer(const std::vector<uint32_t> &recordIndices) {
        for (uint32_t i : recordIndices) { this->recordCommandBuffer(i); }
    }

    void recordCommandBuffer(uint32_t recordIndex = 0) {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        VkCommandBufferBeginInfo commandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        if (vkBeginCommandBuffer(commandBuffers[recordIndex], &commandBufferBeginInfo) != VK_SUCCESS) {  }
        #endif
    }

    void freeCommandBuffer(const std::vector<uint32_t>& freeIndices) {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        std::vector<VkCommandBuffer> commandBuffersToFree{freeIndices.size()};
        for (uint32_t i = 0; i < freeIndices.size(); ++i) { commandBuffersToFree[i] = commandBuffers[freeIndices[i]]; }
        vkFreeCommandBuffers(linkedRenderEngine->device.device, commandPool, static_cast<uint32_t>(commandBuffersToFree.size()), commandBuffersToFree.data());
        #endif
    }

    void freeCommandBuffer(uint32_t freeIndex) {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        vkFreeCommandBuffers(linkedRenderEngine->device.device, commandPool, 1, &commandBuffers[freeIndex]);
        #endif
    }

    #ifdef ILLUMINATION_ENGINE_VULKAN
    VkCommandBuffer operator[](uint32_t index) {
        return commandBuffers[index];
    }
    #endif

    void destroy() const {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (created.commandBuffers) { vkFreeCommandBuffers(linkedRenderEngine->device.device, commandPool, commandBuffers.size(), commandBuffers.data()); }
        if (created.commandPool) { vkDestroyCommandPool(linkedRenderEngine->device.device, commandPool, nullptr); }
        #endif
    }

    ~IECommandPool() {
        destroy();
    }
};