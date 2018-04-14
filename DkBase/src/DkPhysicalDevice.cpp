#include "DkPhysicalDevice.h"
#include "DkApplication.h"
#include "DkWindow.h"

DkPhysicalDevice::DkPhysicalDevice() :
	m_physDevice(VK_NULL_HANDLE),
	m_memProps(),
	m_properties(),
	m_features(),
	m_queueFamilyProps(),
	m_initialized(false)
{}

bool DkPhysicalDevice::init() {
	if (m_initialized) {
		finalize();
	}
	uint numDevices = 0;
	std::vector<VkPhysicalDevice> available;
	if (vkEnumeratePhysicalDevices(getVkInstance(), &numDevices, nullptr) != VK_SUCCESS || numDevices == 0) {
		std::cout << "Failed to find any physical devices." << std::endl;
		return false;
	}
	available.resize(numDevices);
	if (vkEnumeratePhysicalDevices(getVkInstance(), &numDevices, available.data()) != VK_SUCCESS || numDevices == 0) {
		std::cout << "Failed to enumerate physical devices." << std::endl;
		return false;
	}
	m_physDevice = available[0]; // So far, we're expecting to only find one device and use that one.

	// Save memory properties for later
	vkGetPhysicalDeviceMemoryProperties(m_physDevice, &m_memProps);

	// Save device properties for later
	vkGetPhysicalDeviceProperties(m_physDevice, &m_properties);

	// Save device features for later
	vkGetPhysicalDeviceFeatures(m_physDevice, &m_features);

	// Save queue family properties for later
	uint numQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &numQueueFamilies, nullptr);
	if (numQueueFamilies == 0) {
		std::cout << "Failed to identify any queue families." << std::endl;
		return false;
	}
	m_queueFamilyProps.resize(numQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &numQueueFamilies, m_queueFamilyProps.data());
	if (numQueueFamilies == 0) {
		std::cout << "Failed to enumerate queue families." << std::endl;
		return false;
	}
	m_initialized = true;
	return true;
}

void DkPhysicalDevice::finalize() {
	m_physDevice = VK_NULL_HANDLE;
	m_memProps = {};
	m_queueFamilyProps.clear();
	m_initialized = false;
}

int DkPhysicalDevice::findQueueFamilyIndex(VkQueueFlags desirecCapabilities) {
	for (uint index = 0; index < (uint)m_queueFamilyProps.size(); ++index) {
		if ((m_queueFamilyProps[index].queueCount > 0) && (m_queueFamilyProps[index].queueFlags & desirecCapabilities)) {
			return index;
		}
	}
	std::cout << "Failed to identify a queue with the desired capabilities." << std::endl;
	return -1;
}

int DkPhysicalDevice::findQueueFamilyPresentIndex(DkWindow& window) {
	for (uint index = 0; index < (uint)m_queueFamilyProps.size(); ++index) {
		if (m_queueFamilyProps[index].queueCount > 0) {
			VkBool32 presentSupport = VK_FALSE;
			if ((vkGetPhysicalDeviceSurfaceSupportKHR(m_physDevice, index, window.getSurface(), &presentSupport) == VK_SUCCESS)
				&& (presentSupport == VK_TRUE)) {
				return index;
			}
		}
	}
	std::cout << "Failed to identify a queue family with present capabilities." << std::endl;
	return -1;
}