#include "RenderPass.hpp"

#include "CommandBuffer/CommandBuffer.hpp"
#include "RenderEngine.hpp"

IE::Graphics::RenderPass::RenderPass(Preset t_preset) : m_preset(t_preset) {
}

void IE::Graphics::RenderPass::build(
  RenderPassSeries                         *t_renderPassSeries,
  std::vector<VkAttachmentDescription>     &t_attachmentDescriptions,
  std::vector<VkSubpassDescription>        &t_subpassDescriptions,
  std::vector<VkSubpassDependency>         &t_subpassDependency,
  std::vector<IE::Graphics::Image::Preset> &t_attachmentPresets
) {
    m_linkedRenderEngine = t_renderPassSeries->m_linkedRenderEngine;
    m_renderPassSeries   = t_renderPassSeries;

    // Build this render pass
    VkRenderPassCreateInfo renderPassCreateInfo{
      .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext           = nullptr,
      .flags           = 0x0,
      .attachmentCount = static_cast<uint32_t>(t_attachmentDescriptions.size()),
      .pAttachments    = t_attachmentDescriptions.data(),
      .subpassCount    = static_cast<uint32_t>(t_subpassDescriptions.size()),
      .pSubpasses      = t_subpassDescriptions.data(),
      .dependencyCount = static_cast<uint32_t>(t_subpassDependency.size()),
      .pDependencies   = t_subpassDependency.data()};
    VkResult result{
      vkCreateRenderPass(m_linkedRenderEngine->m_device.device, &renderPassCreateInfo, nullptr, &m_renderPass)};
    if (result != VK_SUCCESS)
        m_linkedRenderEngine->getLogger().log(
          "Failed to create Render Pass with error: " + RenderEngine::translateVkResultCodes(result),
          IE::Core::Logger::ILLUMINATION_ENGINE_LOG_LEVEL_ERROR
        );
    else m_linkedRenderEngine->getLogger().log("Created Render Pass");

    // Build all the subpasses controlled by this render pass.
    for (auto &subpass : m_subpasses) subpass->build(this);

    // Build the framebuffer used with this render pass
    m_framebuffer.build(this, t_attachmentPresets);
}

std::shared_ptr<IE::Graphics::CommandBuffer> IE::Graphics::RenderPass::record(std::shared_ptr<IE::Graphics::CommandBuffer> &t_commandBuffer) {
    auto commandBuffer{std::make_shared<IE::Graphics::CommandBuffer>(m_linkedRenderEngine->getCommandPool())};
    int n = 0;
    for (std::shared_ptr<Subpass> &subpass : m_subpasses) subpass->record(t_commandBuffer, n++);
}

IE::Graphics::RenderPass::~RenderPass() {
    destroy();
}

void IE::Graphics::RenderPass::destroy() {
    for (auto &subpass : m_subpasses) subpass->destroy();
    m_framebuffer.destroy();
    if (m_renderPass) {
        vkDestroyRenderPass(m_linkedRenderEngine->m_device.device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }
}
