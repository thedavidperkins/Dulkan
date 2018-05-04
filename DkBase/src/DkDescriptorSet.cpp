#include "DkDescriptorSet.h"
#include "DkDescriptorPool.h"
#include "DkDevice.h"
#include "DkBuffer.h"

DkDescriptorSet::DkDescriptorSet(DkDescriptorPool& pool) :
	m_pool(pool),
	m_set(VK_NULL_HANDLE),
	m_initialized(false)
{}

void DkDescriptorSet::setDescriptorSetHandle(VkDescriptorSet set) {
	m_set = set;
	m_initialized = set != VK_NULL_HANDLE;
}

void DkDescriptorSet::finalize() {
	if (m_set != VK_NULL_HANDLE) {
		vkFreeDescriptorSets(m_pool.getDevice().get(), m_pool.get(), 1, &m_set);
		setDescriptorSetHandle(VK_NULL_HANDLE);
	}
}

bool DkDescriptorSet::updateBuffer(
	uint binding,
	DkBuffer* buffer,
	VkDeviceSize offset,
	VkDeviceSize range,
	VkDescriptorType bufferType
) {
	if (!m_initialized) {
		std::cout << "Cannot update buffer until descriptor set is allocated." << std::endl;
		return false;
	}

	VkDescriptorBufferInfo bufInfo = {
		buffer->get(),
		offset,
		range
	};

	VkWriteDescriptorSet writeInfo = {
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		m_set,
		binding,
		0,
		1,
		bufferType,
		nullptr,
		&bufInfo,
		nullptr
	};

	vkUpdateDescriptorSets(m_pool.getDevice().get(), 1, &writeInfo, 0, nullptr);

	return true;
}