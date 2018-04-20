#include "DkDevice.h"
#include "DkUtils.h"
#include "DkApplication.h"

DkDevice::DkDevice(DkPhysicalDevice& physDevice) :
	m_physDevice(physDevice),
	m_queueIndices(),
	m_desiredExts({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }),
	m_desiredFeatures({}),
	m_device(VK_NULL_HANDLE),
	m_initialized(false)
{
	m_desiredFeatures.geometryShader = VK_TRUE;
}

void DkDevice::setDesiredExts(const std::vector<const char*>& desiredExts) {
	if (m_initialized) {
		std::cout << "Cannot alter desired extensions after initialization." << std::endl;
		return;
	}
	m_desiredExts.clear();
	for (auto& ext : desiredExts) {
		m_desiredExts.push_back(ext);
	}
}

void DkDevice::setQueueIndices(const std::vector<uint>& indices) {
	if (m_initialized) {
		std::cout << "Cannot alter desired queue indices after initialization." << std::endl;
		return;
	}
	m_queueIndices.clear();
	for (auto& ext : indices) {
		m_queueIndices.push_back(ext);
	}
}

bool DkDevice::init() {
	if (m_initialized) {
		finalize();
	}

	// Check for extension support
	std::vector<VkExtensionProperties> available;
	if (!getAvailableDeviceExtensions(m_physDevice, available)) return false;
	for (auto& ext : m_desiredExts) {
		if (!IsExtensionSupported(available, ext)) return false;
	}

	// Check for feature support -- for now, we only check for shaders if desired
	if (m_desiredFeatures.geometryShader == VK_TRUE && m_physDevice.getFeatures().geometryShader != VK_TRUE) {
		std::cout << "Physical device does not support geometry shaders." << std::endl;
		return false;
	}
	if (m_desiredFeatures.tessellationShader == VK_TRUE && m_physDevice.getFeatures().tessellationShader != VK_TRUE) {
		std::cout << "Physical device does not support tessellation shaders." << std::endl;
		return false;
	}

	std::vector<float> priority = { 1.f };
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	for (auto& index : m_queueIndices) {
		queueInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			index,
			1,					// currently assuming one queue per index
			priority.data()		// currently assuming one queue per index
		});
	}

	VkDeviceCreateInfo deviceInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		(uint)queueInfos.size(),
		queueInfos.data(),
		0,							// layer count
		nullptr,					// layer names
		(uint)m_desiredExts.size(),
		m_desiredExts.data(),
		&m_desiredFeatures
	};

	if (vkCreateDevice(getPhysDevice().get(), &deviceInfo, nullptr, &m_device) != VK_SUCCESS) {
		std::cout << "Failed to create logical device." << std::endl;
		return false;
	}

	if (!loadDeviceFns(m_device, m_desiredExts)) return false;

	m_initialized = true;
	return true;
}

void DkDevice::finalize() {
	if (m_device != VK_NULL_HANDLE) {
		vkDestroyDevice(m_device, nullptr);
		m_device = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

bool DkDevice::waitIdle() {
	if (vkDeviceWaitIdle(m_device) != VK_SUCCESS) {
		std::cout << "Device wait failed." << std::endl;
		return false;
	}
	return true;
}