#include "DkImage.h"
#include "DkApplication.h"
#include "DkDevice.h"

DkImage::DkImage(DkDevice& device, VkExtent3D extent, DkDeviceMemory* memory) :
	m_device(device),
	m_extent(extent),
	m_memory(memory),
	m_extMemory(memory != nullptr),
	m_imgCreateFlags((VkImageCreateFlags)0),
	m_imgType(VK_IMAGE_TYPE_2D),
	m_format(VK_FORMAT_B8G8R8A8_UNORM),
	m_mipLevels(1),
	m_arrLayers(1),
	m_sampCount(VK_SAMPLE_COUNT_1_BIT),
	m_tiling(VK_IMAGE_TILING_OPTIMAL),
	m_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
	m_sharing(VK_SHARING_MODE_EXCLUSIVE),
	m_qFamIndices(),
	m_image(VK_NULL_HANDLE),
	m_initialized(false)
{
	if (!m_extMemory) {
		m_memory = new DkDeviceMemory(m_device);
	}
}

void DkImage::setCreateFlags(VkImageCreateFlags flags) {
	if (m_initialized) {
		std::cout << "Cannot alter create flags after initialization." << std::endl;
		return;
	}
	m_imgCreateFlags = flags;
}

void DkImage::setImgType(VkImageType type) {
	if (m_initialized) {
		std::cout << "Cannot alter image type after initialization." << std::endl;
		return;
	}
	m_imgType = type;
}

void DkImage::setFormat(VkFormat format) {
	if (m_initialized) {
		std::cout << "Cannot alter image format after initialization." << std::endl;
		return;
	}
	m_format = format;
}

void DkImage::setMipLevels(uint levels) {
	if (m_initialized) {
		std::cout << "Cannot alter mipmap levels count after initialization." << std::endl;
		return;
	}
	m_mipLevels = levels;
}

void DkImage::setArrLayers(uint layers) {
	if (m_initialized) {
		std::cout << "Cannot alter array layer count after initialization." << std::endl;
		return;
	}
	m_arrLayers = layers;
}

void DkImage::setSampCount(VkSampleCountFlagBits sampCount) {
	if (m_initialized) {
		std::cout << "Cannot alter sample count after initialization." << std::endl;
		return;
	}
	m_sampCount = sampCount;
}

void DkImage::setTiling(VkImageTiling tiling) {
	if (m_initialized) {
		std::cout << "Cannot alter tiling after initialization." << std::endl;
		return;
	}
	m_tiling = tiling;
}

void DkImage::setUsage(VkImageUsageFlags usage) {
	if (m_initialized) {
		std::cout << "Cannot alter usage after initialization." << std::endl;
		return;
	}
	m_usage = usage;
}

void DkImage::setSharing(VkSharingMode sharing) {
	if (m_initialized) {
		std::cout << "Cannot alter sharing mode after initialization." << std::endl;
		return;
	}
	m_sharing = sharing;
}

void DkImage::setQueueFamilies(const std::vector<uint>& indices) {
	if (m_initialized) {
		std::cout << "Cannot alter queues used after initialization." << std::endl;
		return;
	}
	m_qFamIndices = indices;
}

bool DkImage::init() {
	VkImageCreateInfo imgInfo = {
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		m_imgCreateFlags,
		m_imgType,
		m_format,
		m_extent,
		m_mipLevels,
		m_arrLayers,
		m_sampCount,
		m_tiling,
		m_usage,
		m_sharing,
		(uint)m_qFamIndices.size(),
		m_qFamIndices.data()
	};

	if (vkCreateImage(m_device.get(), &imgInfo, nullptr, &m_image) != VK_SUCCESS) {
		std::cout << "Failed to create image." << std::endl;
		return false;
	}

	if (!m_extMemory) {
		m_memory->setOwner(this);
		if (!m_memory->init()) return false;
	}
	else {
		VkMemoryRequirements reqs;
		vkGetImageMemoryRequirements(m_device.get(), m_image, &reqs);
		if (!m_memory->bind(this, reqs.size)) return false;
	}

	if (!m_memory->getMyOffsetAndSize(this, DK_IMAGE_RESOURCE, m_queriedOffset, m_queriedSize)) return false;

	m_initialized = true;
	return true;
}

void DkImage::finalize() {
	if (!m_extMemory) {
		if (m_memory != nullptr) {
			m_memory->finalize();
			delete m_memory;
			m_memory = nullptr;
		}
	}

	if (m_image != VK_NULL_HANDLE) {
		vkDestroyImage(m_device.get(), m_image, nullptr);
		m_image = VK_NULL_HANDLE;
	}

	m_initialized = false;
}