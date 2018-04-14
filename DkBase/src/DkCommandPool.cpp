#include "DkCommandPool.h"
#include "DkCommandBuffer.h"
#include "DkFrameResources.h"

DkCommandPool::DkCommandPool(DkDevice& device, DkQueue& queue) :
	m_device(device),
	m_queue(queue),
	m_parameters(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
	m_commandPool(VK_NULL_HANDLE),
	m_initialized(false)
{}

DkCommandPool::DkCommandPool(DkCommandPool&& rhs) :
	m_device(std::move(rhs.m_device)),
	m_queue(std::move(rhs.m_queue)),
	m_parameters(rhs.m_parameters),
	m_commandPool(rhs.m_commandPool),
	m_initialized(rhs.m_initialized),
	m_allocatedBuffers(std::move(rhs.m_allocatedBuffers))
{}

void DkCommandPool::setParameters(VkCommandPoolCreateFlags params) {
	if (m_initialized) {
		std::cout << "Cannot alter command pool parameters after initialization." << std::endl;
		return;
	}
	m_parameters = params;
}

bool DkCommandPool::init() {
	VkCommandPoolCreateInfo cmdPoolInfo = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		m_parameters,
		m_queue.getFamilyIndex()
	};

	if (vkCreateCommandPool(m_device.get(), &cmdPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		std::cout << "Failed to create command pool." << std::endl;
		return false;
	}

	m_initialized = true;
	return true;
}

bool DkCommandPool::allocate(VkCommandBufferLevel level, uint count, std::vector<DkCommandBuffer*>& allocOut) {
	if (count == 0) {
		std::cout << "Cannot allocate command buffers: no resources requested." << std::endl;
		return false;
	}
	std::vector<VkCommandBuffer> bfrs;
	bfrs.resize(count);

	VkCommandBufferAllocateInfo bfrInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_commandPool,
		level,
		count
	};

	if (vkAllocateCommandBuffers(m_device.get(), &bfrInfo, bfrs.data()) != VK_SUCCESS) {
		std::cout << "Failed to allocate command buffers." << std::endl;
		return false;
	}

	for (auto& bfr : bfrs) {
		DkCommandBuffer* toAdd = new DkCommandBuffer(*this);
		toAdd->setBufferLevel(level);
		toAdd->setBufferHandle(bfr);
		m_allocatedBuffers.push_back(toAdd);
		allocOut.push_back(toAdd);
	}

	return true;
}

bool DkCommandPool::allocate(std::vector<DkFrameResources*>& frames) {
	size_t count = frames.size();
	if (count == 0) {
		std::cout << "Cannot allocate command buffers: no resources requested." << std::endl;
		return false;
	}
	VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // assume primary for framebuffer resources
	std::vector<VkCommandBuffer> bfrs;
	bfrs.resize(frames.size());

	VkCommandBufferAllocateInfo bfrInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_commandPool,
		level,
		(uint)count
	};

	if (vkAllocateCommandBuffers(m_device.get(), &bfrInfo, bfrs.data()) != VK_SUCCESS) {
		std::cout << "Failed to allocate command buffers." << std::endl;
		return false;
	}

	for (size_t iter = 0; iter < count; ++iter) {
		DkCommandBuffer* toAdd = new DkCommandBuffer(*this);
		toAdd->setBufferLevel(level);
		toAdd->setBufferHandle(bfrs[iter]);
		m_allocatedBuffers.push_back(toAdd);
		frames[iter]->setCmdBfr(toAdd);
	}

	return true;
}

DkCommandBuffer* DkCommandPool::allocate(VkCommandBufferLevel level) {
	size_t count = 1;
	
	VkCommandBufferAllocateInfo bfrInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_commandPool,
		level,
		(uint)count
	};

	VkCommandBuffer bfr = VK_NULL_HANDLE;

	if (vkAllocateCommandBuffers(m_device.get(), &bfrInfo, &bfr) != VK_SUCCESS || bfr == VK_NULL_HANDLE) {
		std::cout << "Failed to allocate command buffer." << std::endl;
		return nullptr;
	}

	DkCommandBuffer* toAdd = new DkCommandBuffer(*this);
	toAdd->setBufferLevel(level);
	toAdd->setBufferHandle(bfr);
	m_allocatedBuffers.push_back(toAdd);
	
	return toAdd;
}

void DkCommandPool::finalize() {
	for (auto& bfr : m_allocatedBuffers) {
		bfr->finalize();
		delete bfr;
		bfr = nullptr;
	}
	m_allocatedBuffers.clear();
	
	if (m_commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_device.get(), m_commandPool, nullptr);
		m_commandPool = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

void DkCommandPool::freeBuffer(DkCommandBuffer*& bfr) {
	auto& loc = std::find(m_allocatedBuffers.begin(), m_allocatedBuffers.end(), bfr);
	if (loc != m_allocatedBuffers.end()) {
		m_allocatedBuffers.erase(loc);
		bfr->finalize();
		delete bfr;
		bfr = nullptr;
	}
	else {
		std::cout << "Attempted to free a buffer not owned by this command pool." << std::endl;
	}
}