#include <thread>
#include "IECommandBuffer.hpp"

#include "IEDependency.hpp"
#include "IERenderEngine.hpp"
#include "IEDescriptorSet.hpp"
#include "Core/LogModule/IELogger.hpp"
#include "IEPipeline.hpp"


IECommandBuffer::IECommandBuffer() = default;

void IECommandBuffer::allocate() {
    if (state == IE_COMMAND_BUFFER_STATE_INITIAL) {
        return;
    }
    VkCommandBufferAllocateInfo allocateInfo{
            .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool=commandPool->commandPool,
            .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount=1
    };
    VkResult result = vkAllocateCommandBuffers(linkedRenderEngine->device.device, &allocateInfo, &commandBuffer);
    if (result != VK_SUCCESS) {
        linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Failure to properly allocate command buffers! Error: " + IERenderEngine::translateVkResultCodes(result));
        free();
    } else {
        state = IE_COMMAND_BUFFER_STATE_INITIAL;
    }
}

void IECommandBuffer::record(bool oneTimeSubmit) {
    wait();
    oneTimeSubmission = oneTimeSubmit;
    if (state == IE_COMMAND_BUFFER_STATE_RECORDING) {
        return;
    }
    if (state == IE_COMMAND_BUFFER_STATE_NONE) {
        allocate();
    }
    if (state >= IE_COMMAND_BUFFER_STATE_EXECUTABLE) {
        reset();
    }
    VkCommandBufferBeginInfo beginInfo {
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags=static_cast<VkCommandBufferUsageFlags>(oneTimeSubmission ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0),
    };
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_WARN, "Failure to properly begin command buffer recording! Error: " + IERenderEngine::translateVkResultCodes(result));
    }
    state = IE_COMMAND_BUFFER_STATE_RECORDING;
}

void IECommandBuffer::free() {
    wait();
    vkFreeCommandBuffers(linkedRenderEngine->device.device, commandPool->commandPool, 1, &commandBuffer);
    state = IE_COMMAND_BUFFER_STATE_NONE;
}

IECommandBuffer::IECommandBuffer(IERenderEngine *linkedRenderEngine, IECommandPool *commandPool) {
    create(linkedRenderEngine, commandPool);
}

void IECommandBuffer::reset() {
    wait();
    if (state == IE_COMMAND_BUFFER_STATE_INVALID) {
        linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to reset a command buffer that is invalid!");
    }
    vkResetCommandBuffer(commandBuffer, 0);
    state = IE_COMMAND_BUFFER_STATE_INITIAL;
}

void IECommandBuffer::finish() {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to finish a command buffer that was not recording.");
    }
    vkEndCommandBuffer(commandBuffer);
    state = IE_COMMAND_BUFFER_STATE_EXECUTABLE;
}

void IECommandBuffer::execute(VkSemaphore input, VkSemaphore output, VkFence fence) {
    wait();
//    new std::thread {[&] {
            if (state == IE_COMMAND_BUFFER_STATE_RECORDING) {
                finish();
            }
            else if (state != IE_COMMAND_BUFFER_STATE_EXECUTABLE) {
                linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to execute a command buffer that is not recording or executable!");
            }
            VkSubmitInfo submitInfo {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = input != nullptr,
                .pWaitSemaphores = &input,
                .pWaitDstStageMask = new VkPipelineStageFlags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                .commandBufferCount = 1,
                .pCommandBuffers = &commandBuffer,
                .signalSemaphoreCount = output != nullptr,
                .pSignalSemaphores = &output,
            };
            bool fenceWasNullptr = false;
            if (fence == nullptr) {
                fenceWasNullptr = true;
                VkFenceCreateInfo fenceCreateInfo{
                        .sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
                };
                vkCreateFence(linkedRenderEngine->device.device, &fenceCreateInfo, nullptr, &fence);
            }
            vkResetFences(linkedRenderEngine->device.device, 1, &fence);
            VkResult result = vkQueueSubmit(commandPool->queue, 1, &submitInfo, fence);
            if (result != VK_SUCCESS) {
                linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Failed to submit command buffer! Error: " + IERenderEngine::translateVkResultCodes(result));
            }
            state = IE_COMMAND_BUFFER_STATE_PENDING;
            vkWaitForFences(linkedRenderEngine->device.device, 1, &fence, VK_TRUE, UINT64_MAX);
            state = oneTimeSubmission ? IE_COMMAND_BUFFER_STATE_INVALID : IE_COMMAND_BUFFER_STATE_EXECUTABLE;
            if (fenceWasNullptr) {
                vkDestroyFence(linkedRenderEngine->device.device, fence, nullptr);
            }
            oneTimeSubmission = false;
//    }};
}

void IECommandBuffer::recordPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const std::vector<VkMemoryBarrier>& memoryBarriers, const std::vector<IEBufferMemoryBarrier>& bufferMemoryBarriers, const std::vector<IEImageMemoryBarrier> &imageMemoryBarriers) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a pipeline barrier on a command buffer that is not recording!");
        }
    }
    std::vector<VkBufferMemoryBarrier> bufferBarriers{};
    bufferBarriers.reserve(bufferMemoryBarriers.size());
    for (const IEBufferMemoryBarrier& bufferMemoryBarrier : bufferMemoryBarriers) {
        addDependencies(bufferMemoryBarrier.getDependencies());
        bufferBarriers.push_back((VkBufferMemoryBarrier)bufferMemoryBarrier);
    }
    std::vector<VkImageMemoryBarrier> imageBarriers{};
    imageBarriers.reserve(imageMemoryBarriers.size());
    for (const IEImageMemoryBarrier& imageMemoryBarrier : imageMemoryBarriers) {
        addDependencies(imageMemoryBarrier.getDependencies());
        imageBarriers.push_back((VkImageMemoryBarrier)imageMemoryBarrier);
    }
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarriers.size(), memoryBarriers.data(), bufferBarriers.size(), bufferBarriers.data(), imageBarriers.size(), imageBarriers.data());
}

void IECommandBuffer::recordPipelineBarrier(const IEDependencyInfo *dependencyInfo) {
    if (state == IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a pipeline barrier on a command buffer that is not recording!");
        }
    }
    addDependencies(dependencyInfo->getDependencies());
    vkCmdPipelineBarrier2(commandBuffer, (const VkDependencyInfo *)dependencyInfo);
}

void IECommandBuffer::recordCopyBufferToImage(IEBuffer *buffer, IEImage *image, std::vector<VkBufferImageCopy> regions) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a buffer to image copy on a command buffer that is not recording!");
        }
    }
    addDependencies({image, buffer});
    vkCmdCopyBufferToImage(commandBuffer, buffer->buffer, image->image, image->imageLayout, regions.size(), regions.data());
}

void IECommandBuffer::recordCopyBufferToImage(IECopyBufferToImageInfo *copyInfo) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a buffer to image copy on a command buffer that is not recording!");
        }
    }
    addDependencies(copyInfo->getDependencies());
    vkCmdCopyBufferToImage2(commandBuffer, (const VkCopyBufferToImageInfo2 *)copyInfo);
}

void IECommandBuffer::recordBindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const std::vector<IEBuffer*>& buffers, VkDeviceSize *pOffsets) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a vertex buffer bind on a command buffer that is not recording!");
        }
    }
    VkBuffer pVkBuffers[buffers.size()];
    for (int i = 0; i < buffers.size(); ++i) {
        pVkBuffers[i] = buffers[i]->buffer;
        addDependency(buffers[i]);
    }
    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pVkBuffers, pOffsets);
}

void IECommandBuffer::recordBindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const std::vector<IEBuffer *>& buffers, VkDeviceSize *pOffsets, VkDeviceSize *pSizes, VkDeviceSize *pStrides) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a vertex buffer bind on a command buffer that is not recording!");
        }
    }
    VkBuffer pVkBuffers[buffers.size()];
    for (int i = 0; i < buffers.size(); ++i) {
        pVkBuffers[i] = buffers[i]->buffer;
        addDependency(buffers[i]);
    }
    vkCmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pVkBuffers, pOffsets, pSizes, pStrides);
}

void IECommandBuffer::recordBindIndexBuffer(IEBuffer *buffer, uint32_t offset, VkIndexType indexType) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record an index buffer bind on a command buffer that is not recording!");
        }
    }
    addDependency(buffer);
    vkCmdBindIndexBuffer(commandBuffer, buffer->buffer, offset, indexType);
}

void IECommandBuffer::recordBindPipeline(VkPipelineBindPoint pipelineBindPoint, IEPipeline *pipeline) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a pipeline bind on a command buffer that is not recording!");
        }
    }
    addDependency(pipeline);
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline->pipeline);
}

void IECommandBuffer::recordBindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, IEPipeline *pipeline, uint32_t firstSet, const std::vector<IEDescriptorSet *>& descriptorSets, std::vector<uint32_t> dynamicOffsets) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a descriptor set bind on a command buffer that is not recording!");
        }
    }
    addDependency(pipeline);
    VkDescriptorSet pDescriptorSets[descriptorSets.size()];
    for (int i = 0; i < descriptorSets.size(); ++i) {
        pDescriptorSets[i] = descriptorSets[i]->descriptorSet;
        addDependency(descriptorSets[i]);
    }
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipeline->pipelineLayout, firstSet, descriptorSets.size(), pDescriptorSets, dynamicOffsets.size(), dynamicOffsets.data());
}

void IECommandBuffer::recordDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record an indexed draw on a command buffer that is not recording!");
        }
    }
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void IECommandBuffer::recordBeginRenderPass(IERenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a render pass beginning on a command buffer that is not recording!");
        }
    }
    addDependencies(pRenderPassBegin->getDependencies());
    VkRenderPassBeginInfo renderPassBeginInfo = *(VkRenderPassBeginInfo *)pRenderPassBegin;
    renderPassBeginInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext=pRenderPassBegin->pNext;
    renderPassBeginInfo.renderPass=pRenderPassBegin->renderPass->renderPass;
    renderPassBeginInfo.framebuffer=pRenderPassBegin->framebuffer->framebuffer;
    renderPassBeginInfo.renderArea=pRenderPassBegin->renderArea;
    renderPassBeginInfo.clearValueCount=pRenderPassBegin->clearValueCount;
    renderPassBeginInfo.pClearValues=pRenderPassBegin->pClearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, contents);
}

void IECommandBuffer::recordSetViewport(uint32_t firstViewPort, uint32_t viewPortCount, const VkViewport *pViewPorts) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a viewport set on a command buffer that is not recording!");
        }
    }
    vkCmdSetViewport(commandBuffer, firstViewPort, viewPortCount, pViewPorts);
}

void IECommandBuffer::recordSetScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors) {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a scissor set on a command buffer that is not recording!");
        }
    }
    vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

void IECommandBuffer::recordEndRenderPass() {
    if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
        record();
        if (state != IE_COMMAND_BUFFER_STATE_RECORDING) {
            linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_ERROR, "Attempt to record a render pass ending on a command buffer that is not recording!");
        }
    }
    vkCmdEndRenderPass(commandBuffer);
}

void IECommandBuffer::wait() {
    while (state == IE_COMMAND_BUFFER_STATE_PENDING) {}
}

void IECommandBuffer::destroy() {
    free();
    removeAllDependencies();
}

void IECommandBuffer::create(IERenderEngine *linkedRenderEngine, IECommandPool *commandPool) {
    this->linkedRenderEngine = linkedRenderEngine;
    this->commandPool = commandPool;
    state = IE_COMMAND_BUFFER_STATE_NONE;
}

IECommandBuffer::~IECommandBuffer() {
    destroy();
    commandBuffer = VK_NULL_HANDLE;
}

IECommandBuffer::IECommandBuffer(const IECommandBuffer& source) noexcept: IECommandBuffer(source.linkedRenderEngine, source.commandPool) {
    linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_INFO, "Executing the copy constructor");
}

IECommandBuffer::IECommandBuffer(IECommandBuffer&& source) noexcept : linkedRenderEngine{source.linkedRenderEngine}, commandPool{source.commandPool}, commandBuffer{source.commandBuffer}, state{source.state}, oneTimeSubmission{source.oneTimeSubmission} {
    linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_INFO, "Executing the move constructor");
}
