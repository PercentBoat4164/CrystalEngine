#pragma once

#include <variant>

class Image;

#ifndef ILLUMINATION_ENGINE_VULKAN
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkBuffer);
#endif


class Buffer {
public:
    struct CreateInfo {
        //Only required for buffer
        unsigned int bufferSize{};
        unsigned int usage{};
        #ifdef ILLUMINATION_ENGINE_OPENGL
        unsigned int target{};
        #endif
        #ifdef ILLUMINATION_ENGINE_VULKAN
        VmaMemoryUsage allocationUsage{};

        //Only required for acceleration structure creation
        unsigned int primitiveCount{1};
        VkAccelerationStructureTypeKHR type{};
        VkTransformMatrixKHR *transformationMatrix{};

        //Optional, only available for acceleration structures
        VkAccelerationStructureKHR accelerationStructureToModify{};

        //Only required if type == VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR
        VkDeviceAddress vertexBufferAddress{};
        VkDeviceAddress indexBufferAddress{};
        VkDeviceAddress transformationBufferAddress{};

        //Only required if type == VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR
        std::vector<VkDeviceAddress> bottomLevelAccelerationStructureDeviceAddresses{};
        #endif

        //Optional
        void *data{};

        //Required only if bufferData != nullptr
        unsigned int sizeOfData{};
    };

    void *bufferData{};
    std::variant<VkBuffer, unsigned int> buffer{};
    #ifdef ILLUMINATION_ENGINE_VULKAN
    VkDeviceAddress deviceAddress{};
    VmaAllocation allocation{};
    #endif
    RenderEngineLink *linkedRenderEngine{};
    CreateInfo createdWith{};
    bool created{false};

    virtual Buffer create(RenderEngineLink *engineLink, CreateInfo *createInfo) {
        linkedRenderEngine = engineLink;
        createdWith = *createInfo;
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (linkedRenderEngine->api.name == "Vulkan") {
            VkBufferCreateInfo bufferCreateInfo{.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size=createdWith.bufferSize, .usage=createdWith.usage};
            VmaAllocationCreateInfo allocationCreateInfo{.usage=createdWith.allocationUsage};
            if (vmaCreateBuffer(linkedRenderEngine->allocator, &bufferCreateInfo, &allocationCreateInfo, &std::get<VkBuffer>(buffer), &allocation, nullptr) != VK_SUCCESS) { linkedRenderEngine->log->log("failed to create buffer!", log4cplus::DEBUG_LOG_LEVEL, "Graphics Module"); }
            created = true;
        }
        #endif
        #ifdef ILLUMINATION_ENGINE_OPENGL
        if (linkedRenderEngine->api.name == "OpenGL") {
            glGenBuffers(1, &std::get<unsigned int>(buffer));
            created = true;
        }
        #endif
        if (createdWith.data != nullptr) { upload(createdWith.data, createdWith.sizeOfData); }
        /**@todo: Test this in the Vulkan section above.*/
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (linkedRenderEngine->api.name == "Vulkan") {
            if (createdWith.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
                VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{.sType=VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer=std::get<VkBuffer>(buffer)};
                deviceAddress = linkedRenderEngine->vkGetBufferDeviceAddressKHR(linkedRenderEngine->device.device, &bufferDeviceAddressInfo);
            }
        }
        #endif
        return *this;
    }

    virtual Buffer upload(void *data, unsigned int sizeOfData) {
        if (!created) { linkedRenderEngine->log->log("Called Buffer::upload() on a Buffer that does not exist!", log4cplus::WARN_LOG_LEVEL, "Graphics Module"); }
        if (sizeOfData > createdWith.bufferSize) { linkedRenderEngine->log->log("Buffer::CreateInfo::sizeOfData must not be greater than Buffer::CreateInfo::bufferSize.", log4cplus::WARN_LOG_LEVEL, "Graphics Module"); }
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (linkedRenderEngine->api.name == "Vulkan") {
            vmaMapMemory(linkedRenderEngine->allocator, allocation, &bufferData);
            memcpy(bufferData, data, sizeOfData);
            vmaUnmapMemory(linkedRenderEngine->allocator, allocation);
        }
        #endif
        #ifdef ILLUMINATION_ENGINE_OPENGL
        if (linkedRenderEngine->api.name == "OpenGL") {
            glBindBuffer(createdWith.target, std::get<unsigned int>(buffer));
            glBufferData(createdWith.target, sizeOfData, data, createdWith.usage);
        }
        #endif
        return *this;
    }

    virtual void toImage(Image &image, unsigned int width, unsigned int height, VkCommandBuffer commandBuffer);

    virtual void destroy() {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (linkedRenderEngine->api.name == "Vulkan") { if (created) { vmaDestroyBuffer(linkedRenderEngine->allocator, std::get<VkBuffer>(buffer), allocation); } }
        #endif
        #ifdef ILLUMINATION_ENGINE_OPENGL
        if (linkedRenderEngine->api.name == "OpenGL") { if (created) { glDeleteBuffers(1, &std::get<unsigned int>(buffer)); } }
        #endif
    }
};