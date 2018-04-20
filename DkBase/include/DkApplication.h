#ifndef DK_APPLICATION_H
#define DK_APPLICATION_H

#include <string>
#include <array>
#include "DkCommon.h"
#include "DkInstance.h"
#include "DkWindow.h"
#include "DkPhysicalDevice.h"
#include "DkDevice.h"
#include "DkQueue.h"
#include "DkSwapchain.h"
#include "DkCommandPool.h"
#include "DkFramebuffer.h"
#include "DkFrameResources.h"
#include "DkPipeline.h"
#include "DkMesh.h"

class DkApplication {
public:
	bool vulkanInit();
	void vulkanFinalize();
	virtual bool init() = 0;
	virtual bool draw() = 0;
	virtual void finalize() = 0 { vulkanFinalize(); }
	virtual bool resize() = 0;

	virtual void render();

	virtual ~DkApplication() { finalize(); }

	// Getters
	std::string getAppName() { return m_instance.getAppName(); }
	DkInstance& getDkInstance() { return m_instance; }
	DkPhysicalDevice& getPhysDevice() { return m_physDevice; }
	DkDevice& getDevice() { return m_device; }
	VkDevice getDeviceHandle() { return m_device.get(); }
	DkCommandPool* getCommandPool(DkQueueType type) { return m_commandPools[type]; }
	DkQueue& getQueue(DkQueueType type) { return m_queues[type]; }
	DkWindow& getWindow() { return m_window; }

	// User interaction
	//void MouseClick(size_t button_index, bool state);

	DkApplication();
	DkApplication(const DkApplication& rhs) = delete;
	DkApplication& operator=(const DkApplication& rhs) = delete;
private:
	// Helper member functions
	bool _findQueueIndices(std::vector<uint>& indices);
	void _getDeviceQueues();

	LIBRARY_TYPE m_vkLibrary;
	DkInstance m_instance;
	DkPhysicalDevice m_physDevice;
	DkWindow m_window;
	DkDevice m_device;
	std::array<DkQueue, DK_NUM_QUEUE_TYPES> m_queues;
	std::vector<DkCommandPool*> m_commandPools;
};

#endif//DK_APPLICATION_H
