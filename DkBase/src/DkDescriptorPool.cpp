#include "DkDescriptorPool.h"
#include "DkDevice.h"
#include "DkDescriptorSet.h"

DkDescriptorPool::DkDescriptorPool(DkDevice& device) :
	m_device(device),
	m_flags(0),
	m_maxSets(1),
	m_poolSizes(),
	m_pool(VK_NULL_HANDLE),
	m_initialized(false),
	m_sets()
{}

void DkDescriptorPool::setCreateFlags(VkDescriptorPoolCreateFlags flags) {
	if (m_initialized) {
		std::cout << "Cannot set create flags for descriptor pool after initialization." << std::endl;
		return;
	}
	m_flags = flags;
}

void DkDescriptorPool::setMaxSets(uint maxSets) {
	if (m_initialized) {
		std::cout << "Cannot alter max sets for descriptor pool after initialization." << std::endl;
		return;
	}
	m_maxSets = maxSets;
}

void DkDescriptorPool::addToPoolSize(VkDescriptorType type, uint count) {
	if (m_initialized) {
		std::cout << "Cannot alter pool size for descriptor pool after initialization." << std::endl;
		return;
	}
	m_poolSizes.push_back({ type, count });
}

bool DkDescriptorPool::init() {
	if (m_poolSizes.size() < 1) {
		std::cout << "Cannot initialize descriptor pool without size parameters provided." << std::endl;
		return false;
	}

	VkDescriptorPoolCreateInfo poolInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		m_flags,
		m_maxSets,
		(uint)m_poolSizes.size(),
		m_poolSizes.data()
	};

	if (vkCreateDescriptorPool(m_device.get(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS || m_pool == VK_NULL_HANDLE) {
		std::cout << "Failed to create descriptor pool." << std::endl;
		return false;
	}
	m_initialized = true;
	return true;
}

void DkDescriptorPool::finalize() {
	if (m_pool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(m_device.get(), m_pool, nullptr);
		m_pool = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

DkDescriptorSet* DkDescriptorPool::allocate(VkDescriptorSetLayout layout) {
	VkDescriptorSet handle = VK_NULL_HANDLE;
	VkDescriptorSetAllocateInfo allocInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		m_pool,
		1,
		&layout
	};

	if (vkAllocateDescriptorSets(m_device.get(), &allocInfo, &handle) != VK_SUCCESS) {
		std::cout << "Failed to allocate descriptor set." << std::endl;
		return nullptr;
	}

	DkDescriptorSet* ret = new DkDescriptorSet(*this);
	ret->setDescriptorSetHandle(handle);
	m_sets.push_back(ret);
	return ret;
}