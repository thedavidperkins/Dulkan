#include "DkFence.h"

DkFence::DkFence(DkDevice& device) :
	m_device(device),
	m_fence(VK_NULL_HANDLE)
{}

bool DkFence::init(bool signaled) {
	VkFenceCreateInfo fenceInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		signaled ? VK_FENCE_CREATE_SIGNALED_BIT : (VkFenceCreateFlags)0
	};
	if (vkCreateFence(m_device.get(), &fenceInfo, nullptr, &m_fence) != VK_SUCCESS) {
		std::cout << "Failed to create fence." << std::endl;
		return false;
	}
	m_initialized = true;
	return true;
}

void DkFence::finalize() {
	if (m_fence != VK_NULL_HANDLE) {
		vkDestroyFence(m_device.get(), m_fence, nullptr);
		m_fence = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

bool DkFence::wait(uint timeout) {
	VkResult res = vkWaitForFences(m_device.get(), 1, &m_fence, VK_TRUE, timeout);
	switch (res) {
	case VK_TIMEOUT:
		std::cout << "Fence wait timed out." << std::endl;
		return false;
	case VK_SUCCESS:
		return true;
	default:
		std::cout << "Something went wrong while waiting for fence." << std::endl;
		return false;
	}
}

bool DkFence::reset() {
	if (vkResetFences(m_device.get(), 1, &m_fence) != VK_SUCCESS) {
		std::cout << "Failed to reset fence." << std::endl;
		return false;
	}
	return true;
}