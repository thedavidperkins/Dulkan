#include "DkBuffer.h"
#include "DkDevice.h"
#include "DkDeviceMemory.h"
#include "DkCommandBuffer.h"
#include "DkFence.h"
#include "DkQueue.h"

DkBuffer::DkBuffer(DkDevice& device, DkDeviceMemory* memory) :
	m_device(device),
	m_memory(memory),
	m_extMemory(memory != nullptr),
	m_createFlags(0),
	m_size(0),
	m_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT),
	m_sharingMode(VK_SHARING_MODE_EXCLUSIVE),
	m_qFamIndices(),
	m_buffer(VK_NULL_HANDLE),
	m_initialized(false),
	m_queriedSize(0),
	m_queriedOffset(0)
{
	if (!m_extMemory) {
		m_memory = new DkDeviceMemory(m_device);
	}
}

void DkBuffer::setCreateFlags(VkBufferCreateFlags flags) {
	if (m_initialized) {
		std::cout << "Cannot alter buffer create flags after initialization." << std::endl;
		return;
	}
	m_createFlags = flags;
}

void DkBuffer::setSize(VkDeviceSize size) {
	if (m_initialized) {
		std::cout << "Cannot alter buffer size after initialization." << std::endl;
		return;
	}
	m_size = size;
}

void DkBuffer::setUsage(VkBufferUsageFlags usage) {
	if (m_initialized) {
		std::cout << "Cannot alter buffer usage after initialization." << std::endl;
		return;
	}
	m_usage = usage;
}

void DkBuffer::setSharingMode(VkSharingMode mode) {
	if (m_initialized) {
		std::cout << "Cannot alter sharing mode after initialization." << std::endl;
		return;
	}
	m_sharingMode = mode;
}

void DkBuffer::setQueueFamilies(const std::vector<uint>& indices) {
	if (m_initialized) {
		std::cout << "Cannot alter queues used after initialization." << std::endl;
		return;
	}
	m_qFamIndices = indices;
}

bool DkBuffer::init() {
	if (m_initialized) {
		finalize();
	}

	if (m_size == 0) {
		std::cout << "Cannot initialize a buffer with size zero." << std::endl;
		return false;
	}

	VkBufferCreateInfo bufferInfo = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		m_createFlags,
		m_size,
		m_usage,
		m_sharingMode,
		(uint)m_qFamIndices.size(),
		m_qFamIndices.data()
	};

	if (vkCreateBuffer(m_device.get(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS || m_buffer == VK_NULL_HANDLE) {
		std::cout << "Failed to create buffer." << std::endl;
		return false;
	}

	if (!m_extMemory) {
		m_memory->setOwner(this);
		if (!m_memory->init()) return false;
	}
	else {
		VkMemoryRequirements reqs;
		vkGetBufferMemoryRequirements(m_device.get(), m_buffer, &reqs);
		if (!m_memory->bind(this, reqs.size)) return false;
	}

	if (!m_memory->getMyOffsetAndSize(this, DK_BUFFER_RESOURCE, m_queriedOffset, m_queriedSize)) return false;

	m_initialized = true;
	return true;
}

void DkBuffer::finalize() {
	if (!m_extMemory && m_memory != nullptr) {
		delete m_memory;
		m_memory = nullptr;
	}
	if (m_buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(m_device.get(), m_buffer, nullptr);
		m_buffer = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

// Currently assumes any buffer in regular use is VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT only;
//	An obvious optimization will be to check what memory flag the buffer's bound memory has
//	set, and only go through this staging rigamarole if we can't confirm host visibility
bool DkBuffer::pushData(
	uint size,
	void* data,
	DkCommandBuffer* bfr,
	VkPipelineStageFlags producingStage,
	VkPipelineStageFlags consumingStage,
	VkAccessFlags oldAccess,
	VkAccessFlags newAccess,
	const std::vector<DkSemaphore*>& signalSemaphores,
	DkQueue& queue
) {
	if (!m_initialized) {
		std::cout << "Cannot push data to an uninitialized buffer." << std::endl;
		return false;
	}

	// init staging buffer
	DkBuffer stagingBuffer(m_device, nullptr);
	stagingBuffer.getMemory()->setPropFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	stagingBuffer.setSize(size);
	stagingBuffer.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	if (!stagingBuffer.init()) return false;

	// Map and copy data to staging memory
	void* localPtr = nullptr;
	if (vkMapMemory(m_device.get(), stagingBuffer.getMemory()->get(), 0, VK_WHOLE_SIZE, 0, &localPtr) != VK_SUCCESS ||
		localPtr == nullptr) {
		std::cout << "Failed to map memory range." << std::endl;
		return false;
	}
	std::memcpy(localPtr, data, size);

	// Flush
	VkMappedMemoryRange range = {
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		stagingBuffer.getMemory()->get(),
		0,
		VK_WHOLE_SIZE
	};
	if (vkFlushMappedMemoryRanges(m_device.get(), 1, &range) != VK_SUCCESS) {
		std::cout << "Failed to flush memory." << std::endl;
		return false;
	}

	// Unmap buffer memory
	vkUnmapMemory(m_device.get(), stagingBuffer.getMemory()->get());
	
	// Send copy command to device
	if (!bfr->beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) return false;
	bfr->setMemoryBarrier(producingStage, VK_PIPELINE_STAGE_TRANSFER_BIT, {}, {
		{
			this,
			oldAccess,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED
		}
	}, {});
	bfr->deviceMemCopy(*this, stagingBuffer);
	bfr->setMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, consumingStage, {}, {
		{
			this,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			newAccess,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED
		}
	}, {});
	if (!bfr->endRecording()) return false;
	
	DkFence fence(m_device);
	fence.init(false);
	if (!bfr->submit(queue, {}, signalSemaphores, fence)) return false;
	if (!fence.wait()) return false;
	return true;
}
