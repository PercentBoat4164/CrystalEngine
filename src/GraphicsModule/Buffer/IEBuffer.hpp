#pragma once

/* Predefine classes used with pointers or as return values for functions. */
class IEImage;

class IERenderEngine;

/* Include classes used as attributes or function arguments. */
// Internal dependencies
#include "GraphicsModule/CommandBuffer/IEDependency.hpp"
#include "IEAPI.hpp"

// External dependencies
#include <include/vk_mem_alloc.h>

#define GLEW_IMPLEMENTATION
#include <include/GL/glew.h>

#include <vulkan/vulkan.h>

// System dependencies
#include <vector>
#include <cstdint>
#include <functional>


class IEBuffer : public IEDependency, public std::enable_shared_from_this<IEBuffer> {
public:
	struct CreateInfo {
		uint64_t size{};
		VkBufferUsageFlags usage{};
		VmaMemoryUsage allocationUsage{};

		GLenum type{};
	};

	typedef enum IEBufferStatus {
		IE_BUFFER_STATUS_NONE = 0x0,
		IE_BUFFER_STATUS_UNLOADED = 0x1,
		IE_BUFFER_STATUS_QUEUED_RAM = 0x2,
		IE_BUFFER_STATUS_DATA_IN_RAM = 0x4,
		IE_BUFFER_STATUS_QUEUED_VRAM = 0x8,
		IE_BUFFER_STATUS_DATA_IN_VRAM = 0x10
	} IEBufferStatus;

	std::vector<char> data{};
	VkBuffer buffer{};
	VkDeviceAddress deviceAddress{};
	uint64_t size{};
	VkBufferUsageFlags usage{};
	VmaMemoryUsage allocationUsage{};
	std::vector<std::function<void()>> deletionQueue{};
	IEBufferStatus status{IE_BUFFER_STATUS_NONE};
	GLuint id{};
	GLenum type{};

private:
	static std::function<void(IEBuffer &)> _uploadToRAM;

	static std::function<void(IEBuffer &)> _uploadToVRAM;
	static std::function<void(IEBuffer &, const std::vector<char> &)> _uploadToVRAM_vector;
	static std::function<void(IEBuffer &, void *, std::size_t)> _uploadToVRAM_void;

	static std::function<void(IEBuffer &, const std::vector<char> &)> _update_vector;
	static std::function<void(IEBuffer &, void *, std::size_t)> _update_void;

	static std::function<void(IEBuffer &)> _unloadFromVRAM;

	static std::function<void(IEBuffer &)> _destroy;

protected:
	virtual void _openglUploadToRAM();

	virtual void _vulkanUploadToRAM();


	virtual void _openglUploadToVRAM();

	virtual void _vulkanUploadToVRAM();

	virtual void _openglUploadToVRAM_vector(const std::vector<char> &);

	virtual void _vulkanUploadToVRAM_vector(const std::vector<char> &);

	virtual void _openglUploadToVRAM_void(void *, std::size_t);

	virtual void _vulkanUploadToVRAM_void(void *, std::size_t);


	virtual void _openglUpdate_vector(const std::vector<char> &);

	virtual void _vulkanUpdate_vector(const std::vector<char> &);

	virtual void _openglUpdate_void(void *, std::size_t);

	virtual void _vulkanUpdate_void(void *, std::size_t);


	virtual void _openglUnloadFromVRAM();

	virtual void _vulkanUnloadFromVRAM();


	virtual void _openglDestroy();

	virtual void _vulkanDestroy();

public:
	IEBuffer();

	IEBuffer(IERenderEngine *, IEBuffer::CreateInfo *);

	IEBuffer(IERenderEngine *, uint64_t, VkBufferUsageFlags, VmaMemoryUsage, GLenum);


	static void setAPI(const IEAPI &API);


	void create(IERenderEngine *engineLink, IEBuffer::CreateInfo *createInfo);

	void create(IERenderEngine *, uint64_t, VkBufferUsageFlags, VmaMemoryUsage, GLenum);


	void toImage(const std::shared_ptr<IEImage> &image);


	void uploadToRAM();

	void uploadToRAM(const std::vector<char> &);

	void uploadToRAM(void *, uint64_t);


	virtual void uploadToVRAM();

	void uploadToVRAM(const std::vector<char> &);

	void uploadToVRAM(void *, uint64_t);


	void update(const std::vector<char> &);

	void update(void *, uint64_t);


	void unloadFromVRAM();


	void destroy();


	~IEBuffer() override;

protected:
	IERenderEngine *linkedRenderEngine{};
	VmaAllocation allocation{};
};
