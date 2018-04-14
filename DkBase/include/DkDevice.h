#ifndef DK_DEVICE_H
#define DK_DEVICE_H

#include "DkCommon.h"
#include "DkPhysicalDevice.h"

class DkDevice {
public:
	DkDevice(DkPhysicalDevice& physDevice);
	~DkDevice() { finalize(); }

	bool init();
	void finalize();

	// Setters before init
	void setQueueIndices(const std::vector<uint>& indices);
	void setDesiredExts(const std::vector<const char*>& desiredExts);

	// Special getter for device features struct to be able to directly access and set fields
	VkPhysicalDeviceFeatures& getDesiredFeatures() { return m_desiredFeatures; }

	// Getters
	VkDevice get() { return m_device; }
	DkPhysicalDevice& getPhysDevice() { return m_physDevice; }

	bool waitIdle();
private:
	// On construction
	DkPhysicalDevice& m_physDevice;

	// Before init
	std::vector<uint> m_queueIndices;
	std::vector<const char*> m_desiredExts;
	VkPhysicalDeviceFeatures m_desiredFeatures;

	// Set by init
	VkDevice m_device;
	bool m_initialized;
};

#endif//DK_DEVICE_H
