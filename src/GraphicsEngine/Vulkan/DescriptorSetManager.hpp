#pragma once

#include <deque>
#include <functional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Asset.hpp"
#include "Vertex.hpp"

class DescriptorSetManager {
public:
    VkDescriptorPool descriptorPool{};
    VkDescriptorSet descriptorSet{};

    void destroy() {
        for (const std::function<void()>& function : deletionQueue) { function(); }
        deletionQueue.clear();
    }
    void setup(VulkanGraphicsEngineLink *engineLink, const std::vector<VkDescriptorType>& setupDescriptorTypes, const std::vector<VkShaderStageFlagBits>& setupShaderFlags) {
        linkedRenderEngine = engineLink;
        //create descriptor layout
        if (setupDescriptorTypes.size() != setupShaderFlags.size()) { throw std::runtime_error("number of descriptor types does not equal number of shader flags!"); }
        swapchainImageCount = setupSwapchainImageCount;
        descriptorTypes = setupDescriptorTypes;
        descriptorSetLayoutBindings.clear();
        descriptorPoolSizes.clear();
        descriptorSetLayoutBindings.reserve(setupDescriptorTypes.size());
        descriptorPoolSizes.reserve(setupDescriptorTypes.size());
        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
        descriptorSetLayoutBinding.descriptorCount = 1;
        VkDescriptorPoolSize descriptorPoolSize{};
        descriptorPoolSize.descriptorCount = swapchainImageCount;
        for (unsigned long i = 0; i < setupDescriptorTypes.size(); i++) {
            descriptorSetLayoutBinding.descriptorType = setupDescriptorTypes[i];
            descriptorSetLayoutBinding.stageFlags = setupShaderFlags[i];
            descriptorSetLayoutBinding.binding = i;
            descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
            descriptorPoolSize.type = setupDescriptorTypes[i];
            descriptorPoolSizes.push_back(descriptorPoolSize);
        }
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();
        if (vkCreateDescriptorSetLayout(linkedRenderEngine->device->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) { throw std::runtime_error("failed to create descriptor set layout!"); }
        deletionQueue.emplace_front([&]{ vkDestroyDescriptorSetLayout(linkedRenderEngine->device->device, descriptorSetLayout, nullptr); descriptorSetLayout = VK_NULL_HANDLE; });
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
        descriptorPoolCreateInfo.maxSets = swapchainImageCount;
        if (vkCreateDescriptorPool(linkedRenderEngine->device->device, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) { throw std::runtime_error("failed to create descriptor pool!"); }
        deletionQueue.emplace_front([&]{ vkDestroyDescriptorPool(linkedRenderEngine->device->device, descriptorPool, nullptr); descriptorPool = VK_NULL_HANDLE; });
    }

    void createDescriptorSet(const std::vector<BufferManager>& buffers, const std::vector<ImageManager>& images, const std::vector<bool>& indices) {
        if (buffers.size() + images.size() != indices.size()) { throw std::runtime_error("number of indices does not equal number of images plus number of buffers!"); }
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        vkAllocateDescriptorSets(linkedRenderEngine->device->device, &descriptorSetAllocateInfo, &descriptorSet);
        int bufferCounter{}, imageCounter{};
        std::vector<VkWriteDescriptorSet> descriptorWrites{indices.size()};
        std::vector<VkDescriptorBufferInfo> descriptorBuffers{};
        descriptorBuffers.reserve(buffers.size());
        std::vector<VkDescriptorImageInfo> descriptorImages{};
        descriptorImages.reserve(images.size());
        for (unsigned long i = 0; i < indices.size(); i++) {
            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i].dstSet = descriptorSet;
            descriptorWrites[i].dstBinding = i;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType = descriptorTypes[i];
            descriptorWrites[i].descriptorCount = 1;
            if (!indices[i]) {
                VkDescriptorBufferInfo descriptorBufferInfo{};
                descriptorBufferInfo.buffer = buffers[bufferCounter].buffer;
                descriptorBufferInfo.offset = 0;
                descriptorBufferInfo.range = buffers[bufferCounter].bufferSize;
                descriptorBuffers.push_back(descriptorBufferInfo);
                descriptorWrites[i].pBufferInfo = &descriptorBuffers[bufferCounter];
                bufferCounter++;
            } else {
                VkDescriptorImageInfo descriptorImageInfo{};
                descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                descriptorImageInfo.sampler = images[imageCounter].sampler;
                descriptorImageInfo.imageView = images[imageCounter].view;
                descriptorImages.push_back(descriptorImageInfo);
                descriptorWrites[i].pImageInfo = &descriptorImages[imageCounter];
                imageCounter++;
            }
        }
        vkUpdateDescriptorSets(linkedRenderEngine->device->device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

private:
    VulkanGraphicsEngineLink *linkedRenderEngine{};
    std::deque<std::function<void()>> deletionQueue{};
    std::vector<VkDescriptorType> descriptorTypes{};
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings{};
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes{};
};