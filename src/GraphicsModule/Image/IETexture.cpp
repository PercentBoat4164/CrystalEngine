/* Include this file's header. */
#include "IETexture.hpp"

/* Include dependencies within this module. */
#include "Buffer/IEBuffer.hpp"
#include "IERenderEngine.hpp"

/* Include dependencies from Core. */
#include "Core/LogModule/IELogger.hpp"

#include "assimp/texture.h"


void IETexture::create(IERenderEngine *engineLink, IETexture::CreateInfo *createInfo) {
	linkedRenderEngine = engineLink;
	layout = createInfo->layout;
	format = createInfo->format;
	type = createInfo->type;
	usage = createInfo->usage;

	/**@todo Implement mip-mapping again.*/
	// Determine image data based on settings and input data
//        auto maxMipLevel = static_cast<uint8_t>(std::floor(std::log2(std::max(width, height))) + 1);
//        mipLevels = mipMapping && linkedRenderEngine->settings.mipMapping ? std::min(std::max(maxMipLevel, static_cast<uint8_t>(1)), static_cast<uint8_t>(linkedRenderEngine->settings.mipMapLevel)) : 1;
}

void IETexture::upload(void *data) {
	IEBuffer scratchBuffer{};
	IEBuffer::CreateInfo scratchBufferCreateInfo{};
	scratchBufferCreateInfo.size = static_cast<VkDeviceSize>(width * height) * 4;
	scratchBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	scratchBufferCreateInfo.allocationUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	scratchBuffer.create(linkedRenderEngine, &scratchBufferCreateInfo);
	scratchBuffer.loadFromDiskToRAM(data);
	scratchBuffer.loadFromRAMToVRAM();
	transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	scratchBuffer.toImage(this);
	linkedRenderEngine->graphicsCommandPool[0].execute();
}

void IETexture::upload(IEBuffer *data) {
	transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	data->toImage(this);
}

void IETexture::loadFromDiskToRAM(const aiTexture *texture) {
	int channels;
	if (texture->mHeight == 0) {
		data = (char *) stbi_load_from_memory((unsigned char *) texture->pcData, (int) texture->mWidth, reinterpret_cast<int *>(&width),
											  reinterpret_cast<int *>(&height), &channels, 4);
	} else {
		data = (char *) stbi_load(texture->mFilename.C_Str(), reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height), &channels, 4);
	}
}

void IETexture::loadFromRAMToVRAM() {
	VkImageLayout desiredLayout = layout;
	layout = VK_IMAGE_LAYOUT_UNDEFINED;

	// Set up image create info.
	VkImageCreateInfo imageCreateInfo{
			.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType=type,
			.format=format,
			.extent=VkExtent3D{
					.width=width,
					.height=height,
					.depth=1
			},
			.mipLevels=1, // mipLevels, Unused due to no implementation of mip-mapping support yet.
			.arrayLayers=1,
			.samples=static_cast<VkSampleCountFlagBits>(1),
			.tiling=VK_IMAGE_TILING_OPTIMAL,
			.usage=usage,
			.sharingMode=VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED,
	};

	// Set up allocation create info
	VmaAllocationCreateInfo allocationCreateInfo{
			.usage=allocationUsage,
	};

	// Create image
	if (vmaCreateImage(linkedRenderEngine->allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture image!");
	}
	deletionQueue.emplace_back([&] {
		vmaDestroyImage(linkedRenderEngine->allocator, image, allocation);
	});

	// Set up image view create info.
	VkImageViewCreateInfo imageViewCreateInfo{
			.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image=image,
			.viewType=VK_IMAGE_VIEW_TYPE_2D, /**@todo Add support for more than just 2D images.*/
			.format=format,
			.components=VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
										   VK_COMPONENT_SWIZZLE_IDENTITY},  // Unused. All components are mapped to default data.
			.subresourceRange=VkImageSubresourceRange{
					.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel=0,
					.levelCount=1,  // Unused. Mip-mapping is not yet implemented.
					.baseArrayLayer=0,
					.layerCount=1,
			},
	};

	// Create image view
	if (vkCreateImageView(linkedRenderEngine->device.device, &imageViewCreateInfo, nullptr, &view) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
	deletionQueue.emplace_back([&] {
		vkDestroyImageView(linkedRenderEngine->device.device, view, nullptr);
	});

	// Determine the anisotropy level to use.
//        anisotropyLevel = linkedRenderEngine->settings.anisotropicFilterLevel > 0 * std::min(anisotropyLevel, linkedRenderEngine->device.physical_device.properties.limits.maxSamplerAnisotropy);

	// Set up image sampler create info.
	VkSamplerCreateInfo samplerCreateInfo{
			.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter=VK_FILTER_NEAREST,
			.minFilter=VK_FILTER_NEAREST,
			.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU=VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV=VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW=VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias=linkedRenderEngine->settings->mipMapLevel,
//                .anisotropyEnable=linkedRenderEngine->settings.anisotropicFilterLevel > 0,
//                .maxAnisotropy=anisotropyLevel,
			.compareEnable=VK_FALSE,
			.compareOp=VK_COMPARE_OP_ALWAYS,
			.minLod=0.0F,
//                .maxLod=static_cast<float>(mipLevels),
			.borderColor=VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			.unnormalizedCoordinates=VK_FALSE,
	};

	// Create image sampler
	if (vkCreateSampler(linkedRenderEngine->device.device, &samplerCreateInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
	deletionQueue.emplace_back([&] {
		if (sampler != nullptr) {
			vkDestroySampler(linkedRenderEngine->device.device, sampler, nullptr);
			sampler = VK_NULL_HANDLE;
		}
	});

	// Upload data if provided
	if (data.empty() && dataSource != nullptr) {
		linkedRenderEngine->settings->logger.log(ILLUMINATION_ENGINE_LOG_LEVEL_WARN, "Attempt to create image with raw and buffered data!");
	}
	if (data.empty()) {
		upload(data.data());
	}
	if (dataSource != nullptr) {
		upload(dataSource);
	}

	// Set transition to requested layout from undefined or dst_optimal.
	if (layout != desiredLayout) {
		transitionLayout(desiredLayout);
	}
	linkedRenderEngine->graphicsCommandPool[0].execute();
}

IETexture::IETexture(IERenderEngine *engineLink, IETexture::CreateInfo *createInfo) {
	create(engineLink, createInfo);
}
