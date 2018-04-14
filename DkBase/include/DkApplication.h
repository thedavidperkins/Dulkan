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
	bool init();
	void render();
	void finalize();

	static DkApplication& getInstance() {
		static DkApplication instance;
		return instance;
	}

	~DkApplication() { finalize(); }

	// Getters
	std::string getAppName() { return m_instance.getAppName(); }
	DkInstance& getDkInstance() { return m_instance; }
	DkPhysicalDevice& getPhysDevice() { return m_physDevice; }
	DkDevice& getDevice() { return m_device; }

	// Setters
	void setFrameCount(uint count);

	// User interaction
	//void MouseClick(size_t button_index, bool state);

	bool draw();

	DkApplication(const DkApplication& rhs) = delete;
	DkApplication& operator=(const DkApplication rhs) = delete;
private:
	// Constructor
	DkApplication();

	// Helper member functions
	bool _findQueueIndices(std::vector<uint>& indices);
	void _getDeviceQueues();

	// Options
	uint m_frameCount;

	// Managed objects
	LIBRARY_TYPE m_vkLibrary;
	DkInstance m_instance;
	DkPhysicalDevice m_physDevice;
	DkWindow m_window;
	DkDevice m_device;
	std::array<DkQueue, DK_NUM_QUEUE_TYPES> m_queues;
	DkSwapchain m_swapchain;
	std::vector<DkCommandPool*> m_commandPools;
	std::vector<DkFrameResources*> m_frames;
	uint m_curFrame;
	bool m_initialized;

	DkRenderPass m_renderPass;
	DkMesh* m_triangle;
	DkPipeline m_pipeline;

};

// Helper functions for commonly sought objects
VkInstance getVkInstance();
VkPhysicalDevice getPhysDevice();
VkDevice getDevice();

#endif//DK_APPLICATION_H
