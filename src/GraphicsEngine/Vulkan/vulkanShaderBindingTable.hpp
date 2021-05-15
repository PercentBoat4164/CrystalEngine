#pragma once

class ShaderBindingTableManager : public BufferManager {
public:
    VkStridedDeviceAddressRegionKHR stridedDeviceAddressRegion{};

    void *create(VulkanGraphicsEngineLink *engineLink, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage allocationUsage, uint32_t handleCount) {
        linkedRenderEngine = engineLink;
        bufferSize = size;
        VkBufferCreateInfo bufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = usage;
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.usage = allocationUsage;
        if (vmaCreateBuffer(*linkedRenderEngine->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr) != VK_SUCCESS) { throw std::runtime_error("failed to create buffer"); }
        deletionQueue.emplace_front([&]{ if (buffer != VK_NULL_HANDLE) { vmaDestroyBuffer(*linkedRenderEngine->allocator, buffer, allocation); buffer = VK_NULL_HANDLE; } });
        VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
        bufferDeviceAddressInfo.buffer = buffer;
        deviceAddress = linkedRenderEngine->vkGetBufferDeviceAddressKHR(linkedRenderEngine->device->device, &bufferDeviceAddressInfo);
        const uint32_t handleSizeAligned = (linkedRenderEngine->physicalDeviceInfo->physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize + linkedRenderEngine->physicalDeviceInfo->physicalDeviceRayTracingPipelineProperties.shaderGroupHandleAlignment - 1) & ~(linkedRenderEngine->physicalDeviceInfo->physicalDeviceRayTracingPipelineProperties.shaderGroupHandleAlignment - 1);
        stridedDeviceAddressRegion.deviceAddress = deviceAddress;
        stridedDeviceAddressRegion.stride = handleSizeAligned;
        stridedDeviceAddressRegion.size = handleCount * handleSizeAligned;
        vmaMapMemory(*linkedRenderEngine->allocator, allocation, &data);
        deletionQueue.emplace_front([&]{ if (buffer != VK_NULL_HANDLE) { vmaUnmapMemory(*linkedRenderEngine->allocator, allocation); } });
        return data;
    }
};