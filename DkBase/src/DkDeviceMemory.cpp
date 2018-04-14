#include <algorithm>

#include "DkDeviceMemory.h"
#include "DkDevice.h"
#include "DkImage.h"
#include "DkBuffer.h"

DkDeviceMemory::DkDeviceMemory(DkDevice& device) :
	m_device(device),
	m_img(nullptr),
	m_bfr(nullptr),
	m_reqs({}),
	m_memProps(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	m_devMemory(VK_NULL_HANDLE),
	m_initialized(false),
	m_offset(0),
	m_bindings()
{}

void DkDeviceMemory::setMemReqs(VkMemoryRequirements reqs) {
	if (m_initialized) {
		std::cout << "Cannot alter memory requirements after initialization." << std::endl;
		return;
	}
	m_reqs = reqs;
}

void DkDeviceMemory::setOwner(DkImage* owner) {
	if (m_initialized) {
		std::cout << "Cannot set owner after initialization." << std::endl;
		return;
	}
	m_bfr = nullptr;
	m_img = owner;
}

void DkDeviceMemory::setOwner(DkBuffer* owner) {
	if (m_initialized) {
		std::cout << "Cannot set owner after initialization." << std::endl;
		return;
	}
	m_img = nullptr;
	m_bfr = owner;
}

void DkDeviceMemory::setPropFlags(VkMemoryPropertyFlags flags) {
	if (m_initialized) {
		std::cout << "Cannot alter property flags after initialization." << std::endl;
		return;
	}
	m_memProps = flags;
}

bool DkDeviceMemory::init() {
	bool bindHere = true;
	if (m_img != nullptr) {
		vkGetImageMemoryRequirements(m_device.get(), m_img->get(), &m_reqs);
	}
	else if (m_bfr != nullptr) {
		vkGetBufferMemoryRequirements(m_device.get(), m_bfr->get(), &m_reqs);
	}
	else if (m_reqs.size == 0) {
		std::cout << "Failed to allocate memory: no owning object or memory requirements set." << std::endl;
		return false;
	}
	else {
		bindHere = false;
	}

	VkPhysicalDeviceMemoryProperties devMemProps = m_device.getPhysDevice().getMemProps();
	uint i;
	for (i = 0; i < devMemProps.memoryTypeCount; ++i) {
		if ((m_reqs.memoryTypeBits & (1 << i)) && 
			(devMemProps.memoryTypes[i].propertyFlags & m_memProps) == m_memProps) {
			break;
		}
	}
	if (i == devMemProps.memoryTypeCount) {
		std::cout << "Failed to identify a memory type with the requested properties." << std::endl;
		return false;
	}

	VkMemoryAllocateInfo memInfo = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		m_reqs.size,
		i
	};

	if (vkAllocateMemory(m_device.get(), &memInfo, nullptr, &m_devMemory) != VK_SUCCESS) {
		std::cout << "Failed to allocate memory." << std::endl;
		return false;
	}

	if (bindHere) {
		bool res = true;
		if (m_img != nullptr) {
			res = bind(m_img, m_reqs.size);
		}
		else {
			res = bind(m_bfr, m_reqs.size);
		}
		if (!res) return false;
	}

	m_initialized = true;
	return true;
}

bool DkDeviceMemory::bind(DkImage* img, VkDeviceSize size) {
	if (vkBindImageMemory(m_device.get(), img->get(), m_devMemory, m_offset) != VK_SUCCESS) {
		std::cout << "Failed to bind image memory." << std::endl;
		return false;
	}
	VkDeviceSize start = m_offset;
	m_offset += size;
	m_bindings.emplace_back();
	DkMemoryResource r;
	r.img = img;
	m_bindings.back() = {
		start,
		size,
		DK_IMAGE_RESOURCE,
		r
	};
	return true;
}

bool DkDeviceMemory::bind(DkBuffer* bfr, VkDeviceSize size) {
	if (vkBindBufferMemory(m_device.get(), bfr->get(), m_devMemory, m_offset) != VK_SUCCESS) {
		std::cout << "Failed to bind buffer memory." << std::endl;
		return false;
	}
	VkDeviceSize start = m_offset;
	m_offset += size;
	m_bindings.emplace_back();
	DkMemoryResource r;
	r.bfr = bfr;
	m_bindings.back() = {
		start,
		size,
		DK_BUFFER_RESOURCE,
		r
	};
	return true;
}

void DkDeviceMemory::finalize() {
	m_bindings.clear();
	m_offset = 0;
	m_bfr = nullptr;
	m_img = nullptr;
	m_reqs = {};
	if (m_devMemory != VK_NULL_HANDLE) {
		vkFreeMemory(m_device.get(), m_devMemory, nullptr);
		m_devMemory = VK_NULL_HANDLE;
	}
}

bool DkDeviceMemory::getMyOffsetAndSize(const void* resource, DkResourceType type, VkDeviceSize& offset, VkDeviceSize& size) {
	auto& me = std::find_if(m_bindings.begin(), m_bindings.end(), [resource, type](const DkBindLog& item) {
		if (item.type != type) return false;
		return (type == DK_BUFFER_RESOURCE ? (resource == item.resource.bfr) : (resource == item.resource.img));
	});
	if (me != m_bindings.end()) {
		offset = me->start;
		size = me->size;
		return true;
	}
	std::cout << "Couldn't find resource in memory bindings." << std::endl;
	return false;
}