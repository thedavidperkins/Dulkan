#ifndef DK_PHYSICAL_DEVICE_H
#define DK_PHYSICAL_DEVICE_H

#include "DkCommon.h"

class DkWindow;

class DkPhysicalDevice {
public:
	DkPhysicalDevice();
	~DkPhysicalDevice() { finalize(); }

	bool init();
	void finalize();

	int findQueueFamilyIndex(VkQueueFlags desiredCapabilities);
	int findQueueFamilyPresentIndex(DkWindow& window);

	// Getters
	VkPhysicalDevice get() { return m_physDevice; }
	VkPhysicalDeviceFeatures getFeatures() const { return m_features; }
	VkPhysicalDeviceMemoryProperties getMemProps() const { return m_memProps; }

	DkPhysicalDevice(const DkPhysicalDevice& rhs) = delete;
	DkPhysicalDevice& operator=(const DkPhysicalDevice& rhs) = delete;
private:
	// set by init
	VkPhysicalDevice m_physDevice;
	VkPhysicalDeviceMemoryProperties m_memProps;
	VkPhysicalDeviceFeatures m_features;
	VkPhysicalDeviceProperties m_properties;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProps;
	bool m_initialized;
};

#endif//DK_PHYSICAL_DEVICE_H
