#include "DkSemaphore.h"

DkSemaphore::DkSemaphore(DkDevice& device) :
	m_device(device),
	m_semaphore(VK_NULL_HANDLE)
{}

bool DkSemaphore::init() {
	VkSemaphoreCreateInfo semInfo = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};
	if (vkCreateSemaphore(m_device.get(), &semInfo, nullptr, &m_semaphore) != VK_SUCCESS) {
		std::cout << "Failed to create semaphore." << std::endl;
		return false;
	}
	m_initialized = true;
	return true;
}

void DkSemaphore::finalize() {
	if (m_semaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(m_device.get(), m_semaphore, nullptr);
		m_semaphore = VK_NULL_HANDLE;
	}
	m_initialized = false;
}