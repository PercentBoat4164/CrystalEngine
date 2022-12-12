#include "ImageVulkan.hpp"

#include "RenderEngine.hpp"

VkFormat formats[]{
  VK_FORMAT_UNDEFINED,
  VK_FORMAT_R4G4_UNORM_PACK8,
  VK_FORMAT_R4G4B4A4_UNORM_PACK16,
  VK_FORMAT_B4G4R4A4_UNORM_PACK16,
  VK_FORMAT_R5G6B5_UNORM_PACK16,
  VK_FORMAT_B5G6R5_UNORM_PACK16,
  VK_FORMAT_R5G5B5A1_UNORM_PACK16,
  VK_FORMAT_B5G5R5A1_UNORM_PACK16,
  VK_FORMAT_A1R5G5B5_UNORM_PACK16,
  VK_FORMAT_R8_UNORM,
  VK_FORMAT_R8_SNORM,
  VK_FORMAT_R8_USCALED,
  VK_FORMAT_R8_SSCALED,
  VK_FORMAT_R8_UINT,
  VK_FORMAT_R8_SINT,
  VK_FORMAT_R8_SRGB,
  VK_FORMAT_R8G8_UNORM,
  VK_FORMAT_R8G8_SNORM,
  VK_FORMAT_R8G8_USCALED,
  VK_FORMAT_R8G8_SSCALED,
  VK_FORMAT_R8G8_UINT,
  VK_FORMAT_R8G8_SINT,
  VK_FORMAT_R8G8_SRGB,
  VK_FORMAT_R8G8B8_UNORM,
  VK_FORMAT_R8G8B8_SNORM,
  VK_FORMAT_R8G8B8_USCALED,
  VK_FORMAT_R8G8B8_SSCALED,
  VK_FORMAT_R8G8B8_UINT,
  VK_FORMAT_R8G8B8_SINT,
  VK_FORMAT_R8G8B8_SRGB,
  VK_FORMAT_B8G8R8_UNORM,
  VK_FORMAT_B8G8R8_SNORM,
  VK_FORMAT_B8G8R8_USCALED,
  VK_FORMAT_B8G8R8_SSCALED,
  VK_FORMAT_B8G8R8_UINT,
  VK_FORMAT_B8G8R8_SINT,
  VK_FORMAT_B8G8R8_SRGB,
  VK_FORMAT_R8G8B8A8_UNORM,
  VK_FORMAT_R8G8B8A8_SNORM,
  VK_FORMAT_R8G8B8A8_USCALED,
  VK_FORMAT_R8G8B8A8_SSCALED,
  VK_FORMAT_R8G8B8A8_UINT,
  VK_FORMAT_R8G8B8A8_SINT,
  VK_FORMAT_R8G8B8A8_SRGB,
  VK_FORMAT_B8G8R8A8_UNORM,
  VK_FORMAT_B8G8R8A8_SNORM,
  VK_FORMAT_B8G8R8A8_USCALED,
  VK_FORMAT_B8G8R8A8_SSCALED,
  VK_FORMAT_B8G8R8A8_UINT,
  VK_FORMAT_B8G8R8A8_SINT,
  VK_FORMAT_B8G8R8A8_SRGB,
  VK_FORMAT_A8B8G8R8_UNORM_PACK32,
  VK_FORMAT_A8B8G8R8_SNORM_PACK32,
  VK_FORMAT_A8B8G8R8_USCALED_PACK32,
  VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
  VK_FORMAT_A8B8G8R8_UINT_PACK32,
  VK_FORMAT_A8B8G8R8_SINT_PACK32,
  VK_FORMAT_A8B8G8R8_SRGB_PACK32,
  VK_FORMAT_A2R10G10B10_UNORM_PACK32,
  VK_FORMAT_A2R10G10B10_SNORM_PACK32,
  VK_FORMAT_A2R10G10B10_USCALED_PACK32,
  VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
  VK_FORMAT_A2R10G10B10_UINT_PACK32,
  VK_FORMAT_A2R10G10B10_SINT_PACK32,
  VK_FORMAT_A2B10G10R10_UNORM_PACK32,
  VK_FORMAT_A2B10G10R10_SNORM_PACK32,
  VK_FORMAT_A2B10G10R10_USCALED_PACK32,
  VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
  VK_FORMAT_A2B10G10R10_UINT_PACK32,
  VK_FORMAT_A2B10G10R10_SINT_PACK32,
  VK_FORMAT_R16_UNORM,
  VK_FORMAT_R16_SNORM,
  VK_FORMAT_R16_USCALED,
  VK_FORMAT_R16_SSCALED,
  VK_FORMAT_R16_UINT,
  VK_FORMAT_R16_SINT,
  VK_FORMAT_R16_SFLOAT,
  VK_FORMAT_R16G16_UNORM,
  VK_FORMAT_R16G16_SNORM,
  VK_FORMAT_R16G16_USCALED,
  VK_FORMAT_R16G16_SSCALED,
  VK_FORMAT_R16G16_UINT,
  VK_FORMAT_R16G16_SINT,
  VK_FORMAT_R16G16_SFLOAT,
  VK_FORMAT_R16G16B16_UNORM,
  VK_FORMAT_R16G16B16_SNORM,
  VK_FORMAT_R16G16B16_USCALED,
  VK_FORMAT_R16G16B16_SSCALED,
  VK_FORMAT_R16G16B16_UINT,
  VK_FORMAT_R16G16B16_SINT,
  VK_FORMAT_R16G16B16_SFLOAT,
  VK_FORMAT_R16G16B16A16_UNORM,
  VK_FORMAT_R16G16B16A16_SNORM,
  VK_FORMAT_R16G16B16A16_USCALED,
  VK_FORMAT_R16G16B16A16_SSCALED,
  VK_FORMAT_R16G16B16A16_UINT,
  VK_FORMAT_R16G16B16A16_SINT,
  VK_FORMAT_R16G16B16A16_SFLOAT,
  VK_FORMAT_R32_UINT,
  VK_FORMAT_R32_SINT,
  VK_FORMAT_R32_SFLOAT,
  VK_FORMAT_R32G32_UINT,
  VK_FORMAT_R32G32_SINT,
  VK_FORMAT_R32G32_SFLOAT,
  VK_FORMAT_R32G32B32_UINT,
  VK_FORMAT_R32G32B32_SINT,
  VK_FORMAT_R32G32B32_SFLOAT,
  VK_FORMAT_R32G32B32A32_UINT,
  VK_FORMAT_R32G32B32A32_SINT,
  VK_FORMAT_R32G32B32A32_SFLOAT,
  VK_FORMAT_R64_UINT,
  VK_FORMAT_R64_SINT,
  VK_FORMAT_R64_SFLOAT,
  VK_FORMAT_R64G64_UINT,
  VK_FORMAT_R64G64_SINT,
  VK_FORMAT_R64G64_SFLOAT,
  VK_FORMAT_R64G64B64_UINT,
  VK_FORMAT_R64G64B64_SINT,
  VK_FORMAT_R64G64B64_SFLOAT,
  VK_FORMAT_R64G64B64A64_UINT,
  VK_FORMAT_R64G64B64A64_SINT,
  VK_FORMAT_R64G64B64A64_SFLOAT,
  VK_FORMAT_B10G11R11_UFLOAT_PACK32,
  VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
  VK_FORMAT_D16_UNORM,
  VK_FORMAT_X8_D24_UNORM_PACK32,
  VK_FORMAT_D32_SFLOAT,
  VK_FORMAT_S8_UINT,
  VK_FORMAT_D16_UNORM_S8_UINT,
  VK_FORMAT_D24_UNORM_S8_UINT,
  VK_FORMAT_D32_SFLOAT_S8_UINT,
  VK_FORMAT_BC1_RGB_UNORM_BLOCK,
  VK_FORMAT_BC1_RGB_SRGB_BLOCK,
  VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
  VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
  VK_FORMAT_BC2_UNORM_BLOCK,
  VK_FORMAT_BC2_SRGB_BLOCK,
  VK_FORMAT_BC3_UNORM_BLOCK,
  VK_FORMAT_BC3_SRGB_BLOCK,
  VK_FORMAT_BC4_UNORM_BLOCK,
  VK_FORMAT_BC4_SNORM_BLOCK,
  VK_FORMAT_BC5_UNORM_BLOCK,
  VK_FORMAT_BC5_SNORM_BLOCK,
  VK_FORMAT_BC6H_UFLOAT_BLOCK,
  VK_FORMAT_BC6H_SFLOAT_BLOCK,
  VK_FORMAT_BC7_UNORM_BLOCK,
  VK_FORMAT_BC7_SRGB_BLOCK,
  VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
  VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
  VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
  VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
  VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
  VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
  VK_FORMAT_EAC_R11_UNORM_BLOCK,
  VK_FORMAT_EAC_R11_SNORM_BLOCK,
  VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
  VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
  VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
  VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
  VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
  VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
  VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
  VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
  VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
  VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
  VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
  VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
  VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
  VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
  VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
  VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
  VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
  VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
  VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
  VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
  VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
  VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
  VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
  VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
  VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
  VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
  VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
  VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
  VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
  VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
  VK_FORMAT_G8B8G8R8_422_UNORM,
  VK_FORMAT_B8G8R8G8_422_UNORM,
  VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
  VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
  VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
  VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
  VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
  VK_FORMAT_R10X6_UNORM_PACK16,
  VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
  VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
  VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
  VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
  VK_FORMAT_R12X4_UNORM_PACK16,
  VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
  VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
  VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
  VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
  VK_FORMAT_G16B16G16R16_422_UNORM,
  VK_FORMAT_B16G16R16G16_422_UNORM,
  VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
  VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
  VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
  VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
  VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
  VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
  VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,
  VK_FORMAT_A4R4G4B4_UNORM_PACK16,
  VK_FORMAT_A4B4G4R4_UNORM_PACK16,
  VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK,
  VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK,
  VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
  VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
  VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
  VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
  VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
  VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
  VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
  VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
  VK_FORMAT_R16G16_S10_5_NV,
  VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT,
  VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT,
  VK_FORMAT_G8B8G8R8_422_UNORM_KHR,
  VK_FORMAT_B8G8R8G8_422_UNORM_KHR,
  VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR,
  VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR,
  VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR,
  VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR,
  VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR,
  VK_FORMAT_R10X6_UNORM_PACK16_KHR,
  VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR,
  VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR,
  VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR,
  VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR,
  VK_FORMAT_R12X4_UNORM_PACK16_KHR,
  VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR,
  VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR,
  VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR,
  VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR,
  VK_FORMAT_G16B16G16R16_422_UNORM_KHR,
  VK_FORMAT_B16G16R16G16_422_UNORM_KHR,
  VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR,
  VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR,
  VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR,
  VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR,
  VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR,
  VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT,
  VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT,
  VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,
  VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT,
};

const VkImageLayout IE::Graphics::detail::ImageVulkan::m_layouts[]{
  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,         // IE_IMAGE_PRESET_CUSTOM
  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,         // IE_IMAGE_PRESET_FRAMEBUFFER_COLOR
  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // IE_IMAGE_PRESET_FRAMEBUFFER_DEPTH
};

const VkFormatFeatureFlags IE::Graphics::detail::ImageVulkan::m_features[]{
  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,         // IE_IMAGE_PRESET_CUSTOM
  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,         // IE_IMAGE_PRESET_FRAMEBUFFER_COLOR
  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT  // IE_IMAGE_PRESET_FRAMEBUFFER_DEPTH
};

const VkPipelineStageFlags IE::Graphics::detail::ImageVulkan::m_stages[]{
  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // IE_IMAGE_PRESET_CUSTOM
  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // IE_IMAGE_PRESET_FRAMEBUFFER_COLOR
  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT      // IE_IMAGE_PRESET_FRAMEBUFFER_DEPTH
};

const VkAccessFlags IE::Graphics::detail::ImageVulkan::m_accessFlags[]{
  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // IE_IMAGE_PRESET_CUSTOM
  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // IE_IMAGE_PRESET_FRAMEBUFFER_COLOR
  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT  // IE_IMAGE_PRESET_FRAMEBUFFER_DEPTH
};

const IE::Graphics::Image::Intent IE::Graphics::detail::ImageVulkan::m_intents[]{
  IE_IMAGE_INTENT_COLOR,  // IE_IMAGE_PRESET_CUSTOM
  IE_IMAGE_INTENT_COLOR,  // IE_IMAGE_PRESET_FRAMEBUFFER_COLOR
  IE_IMAGE_INTENT_DEPTH   // IE_IMAGE_PRESET_FRAMEBUFFER_DEPTH
};

VkImageLayout IE::Graphics::detail::ImageVulkan::layoutFromPreset(IE::Graphics::Image::Preset t_preset) {
    return m_layouts[t_preset];
}

VkFormatFeatureFlags IE::Graphics::detail::ImageVulkan::featuresFromPreset(IE::Graphics::Image::Preset t_preset) {
    return m_features[t_preset];
}

VkFormat IE::Graphics::detail::ImageVulkan::formatFromPreset(
  IE::Graphics::Image::Preset t_preset,
  IE::Graphics::RenderEngine *t_engineLink
) {
    return intentFromPreset(t_preset) == IE_IMAGE_INTENT_COLOR ? VK_FORMAT_B8G8R8A8_SRGB :
                                                                 VK_FORMAT_D32_SFLOAT_S8_UINT;
    //    VkFormatFeatureFlags featureFlags{featuresFromPreset(t_preset)};
    //    // Try for an optimal format.
    //    for (VkFormat format : formats) {
    //        VkFormatProperties properties;
    //        vkGetPhysicalDeviceFormatProperties(t_engineLink->m_device.physical_device, format, &properties);
    //        if (properties.optimalTilingFeatures & featureFlags) return format;
    //    }
    //    // Try for any compatible format.
    //    for (VkFormat format : formats) {
    //        VkFormatProperties properties;
    //        vkGetPhysicalDeviceFormatProperties(t_engineLink->m_device.physical_device, format, &properties);
    //        if (properties.optimalTilingFeatures & featureFlags) return format;
    //    }
    //    return VK_FORMAT_UNDEFINED;
}

VkPipelineStageFlags IE::Graphics::detail::ImageVulkan::stageFromPreset(Preset t_preset) {
    return m_stages[t_preset];
}

VkAccessFlags IE::Graphics::detail::ImageVulkan::accessFlagsFromPreset(IE::Graphics::Image::Preset t_preset) {
    return m_accessFlags[t_preset];
}

IE::Graphics::Image::Intent IE::Graphics::detail::ImageVulkan::intentFromPreset(Preset t_preset) {
    return m_intents[t_preset];
}

bool IE::Graphics::detail::ImageVulkan::_createImage(
  IE::Graphics::Image::Preset                      t_preset,
  uint64_t                                         t_flags,
  IE::Core::MultiDimensionalVector<unsigned char> &t_data
) {
    m_preset = t_preset;

    std::vector<uint32_t> queueFamilyIndices{
      m_linkedRenderEngine->m_device.get_queue_index(vkb::QueueType::graphics).value(),
    };

    VkImageCreateInfo imageCreateInfo{
      .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext     = nullptr,
      .flags     = 0x0,
      .imageType = VK_IMAGE_TYPE_2D,
      .format    = formatFromPreset(m_preset, m_linkedRenderEngine),
      .extent =
        {.width  = static_cast<uint32_t>(m_linkedRenderEngine->m_currentResolution[0]),
                 .height = static_cast<uint32_t>(m_linkedRenderEngine->m_currentResolution[1]),
                 .depth  = 1},
      .mipLevels   = 0x1,
      .arrayLayers = 0x1,
      .samples     = VK_SAMPLE_COUNT_1_BIT,
      .tiling      = VK_IMAGE_TILING_OPTIMAL,
      .usage       = intentFromPreset(m_preset) == IE_IMAGE_INTENT_COLOR ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT :
                                                                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
      .pQueueFamilyIndices   = queueFamilyIndices.data(),
      .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocationCreateInfo{
      .flags          = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
      .usage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      .requiredFlags  = 0x0,
      .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      .memoryTypeBits = UINT32_MAX,
      .pool           = nullptr,
      .pUserData      = dynamic_cast<Image *>(this),
      .priority       = 0.0f};

    VkResult result{vmaCreateImage(
      m_linkedRenderEngine->getAllocator(),
      &imageCreateInfo,
      &allocationCreateInfo,
      &m_id,
      &m_allocation,
      nullptr
    )};
    if (result != VK_SUCCESS) {
        m_linkedRenderEngine->getLogger().log(
          "Failed to create image! Error: " + IE::Graphics::RenderEngine::translateVkResultCodes(result)
        );
        return false;
    }
    m_linkedRenderEngine->getLogger().log("Created Image");

    // Create view
    VkImageViewCreateInfo imageViewCreateInfo{
      .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext    = nullptr,
      .flags    = 0x0,
      .image    = m_id,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format   = formatFromPreset(m_preset, m_linkedRenderEngine),
      .components =
        VkComponentMapping{
                           .r = VK_COMPONENT_SWIZZLE_R,
                           .g = VK_COMPONENT_SWIZZLE_G,
                           .b = VK_COMPONENT_SWIZZLE_B,
                           .a = VK_COMPONENT_SWIZZLE_A},
      .subresourceRange = VkImageSubresourceRange{
                           .aspectMask     = intentFromPreset(m_preset) == IE_IMAGE_INTENT_COLOR ? VK_IMAGE_ASPECT_COLOR_BIT :
                                                                                VK_IMAGE_ASPECT_DEPTH_BIT, .baseMipLevel   = 0,
                           .levelCount     = VK_REMAINING_MIP_LEVELS,
                           .baseArrayLayer = 0,
                           .layerCount     = VK_REMAINING_ARRAY_LAYERS}
    };
    result = vkCreateImageView(m_linkedRenderEngine->m_device.device, &imageViewCreateInfo, nullptr, &m_view);
    if (result != VK_SUCCESS) {
        m_linkedRenderEngine->getLogger().log(
          "Failed to create image view! Error: " + IE::Graphics::RenderEngine::translateVkResultCodes(result)
        );
        return false;
    }
    m_linkedRenderEngine->getLogger().log("Created Image View");
    return true;
}

bool IE::Graphics::detail::ImageVulkan::_destroyImage() {
    vmaDestroyImage(m_linkedRenderEngine->getAllocator(), m_id, m_allocation);
    return false;
}
