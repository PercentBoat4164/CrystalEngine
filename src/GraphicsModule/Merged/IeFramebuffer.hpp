#pragma once

#include <vector>

#include "IeImage.hpp"
#include "IeRenderEngineLink.hpp"
#include "IeRenderPass.hpp"
#include "IeFramebufferAttachment.hpp"

enum IeFramebufferAttachmentFormat {
    IE_FRAMEBUFFER_DEPTH_ATTACHMENT_FORMAT = VK_FORMAT_D32_SFLOAT_S8_UINT
};

struct IeRenderPassAttachmentDescription {
    VkAttachmentDescription depth{};
    std::vector<VkAttachmentDescription> color{};
    std::vector<VkAttachmentDescription> resolve{};
};

class IeRenderPass;

class IeFramebuffer {
public:
    struct CreateInfo {
        IeFramebufferAspect aspects{IE_FRAMEBUFFER_ASPECT_DEPTH_AND_COLOR};
        uint8_t msaaSamples{1};
        VkImageView swapchainImageView;
        IeImageFormat format;
        IeFramebuffer* dependentOn;
        IeFramebuffer* requiredBy;
        uint32_t subpass{0};
        uint32_t colorImageCount{1};
    };

    struct Created {
        bool framebuffer{};
    };

    CreateInfo createdWith{};
    Created created{};
    IeFramebufferAttachment depth{};
    std::vector<IeFramebufferAttachment> colorAttachments{};
    std::vector<IeFramebufferAttachment> resolveAttachments{};
    std::vector<VkFramebuffer> framebuffers{};
    std::vector<VkClearValue> clearValues{3};
    IeRenderEngineLink *linkedRenderEngine{};



    void create(IeRenderEngineLink *engineLink, IeFramebuffer::CreateInfo *createInfo) {

    }

    void linkToRenderPass(IeRenderPass* renderPass) {

    }

    /**
     * @brief Generates the necessary data to link this framebuffers to a renderpass.
     * @param createInfo
     */
    static IeRenderPassAttachmentDescription generateAttachmentDescriptions(IeRenderEngineLink* linkedRenderEngine, IeFramebuffer::CreateInfo* createInfo) {
        IeRenderPassAttachmentDescription renderPassAttachmentDescription{}; // prepare result data
        if (createInfo->aspects & IE_FRAMEBUFFER_ASPECT_COLOR_BIT) {
            if (createInfo->colorImageCount == 0) {
                linkedRenderEngine->log->log("Color bit set, but requested 0 color images. Creating one color image anyway.", log4cplus::WARN_LOG_LEVEL, "Graphics Module");
            }
            createInfo->colorImageCount = std::max(1u, createInfo->colorImageCount);
            auto format = ieImageFormats.find(createInfo->format);
            if (format == ieImageFormats.end()) {
                return renderPassAttachmentDescription;
            }
            renderPassAttachmentDescription.color = {
                    createInfo->colorImageCount,
                    VkAttachmentDescription{
                            .format=format->second.first,
                            .samples=static_cast<VkSampleCountFlagBits>(createInfo->msaaSamples),
                            .loadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .stencilLoadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .stencilStoreOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout=VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout=createInfo->requiredBy ? VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                    }
            };
            if (createInfo->msaaSamples > 1) {
                renderPassAttachmentDescription.resolve = {
                        createInfo->colorImageCount,
                        VkAttachmentDescription{
                                .format=format->second.first,
                                .samples=VK_SAMPLE_COUNT_1_BIT,
                                .loadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                                .storeOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                .stencilLoadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                                .stencilStoreOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                .initialLayout=VK_IMAGE_LAYOUT_UNDEFINED,
                                .finalLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                        }
                };
            }
        }
        if (createInfo->aspects & IE_FRAMEBUFFER_ASPECT_DEPTH_BIT) {
            renderPassAttachmentDescription.depth = VkAttachmentDescription{
                    .format=static_cast<VkFormat>(IE_FRAMEBUFFER_DEPTH_ATTACHMENT_FORMAT),
                    .samples=static_cast<VkSampleCountFlagBits>(createInfo->msaaSamples),
                    .loadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .stencilLoadOp=createInfo->dependentOn ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .stencilStoreOp=createInfo->requiredBy ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout=VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };
        }
        return renderPassAttachmentDescription;
    }

    void destroy() {
        if (created.framebuffer) {
            for (VkFramebuffer framebuffer : framebuffers) {
                vkDestroyFramebuffer(linkedRenderEngine->device.device, framebuffer, nullptr);
            }
        }
    }

    ~IeFramebuffer() {
        destroy();
    }
};