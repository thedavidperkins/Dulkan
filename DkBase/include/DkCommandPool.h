#ifndef DK_COMMAND_POOL_H
#define DK_COMMAND_POOL_H

#include "DkCommon.h"
#include "DkDevice.h"
#include "DkQueue.h"

class DkCommandBuffer;
class DkFrameResources;

class DkCommandPool {
public:
	DkCommandPool(DkDevice& device, DkQueue& queue);
	DkCommandPool(DkCommandPool&& rhs); // move constructor
	~DkCommandPool() { finalize(); }
	
	bool init();
	void finalize();

	// Getters
	VkCommandPool& get() { return m_commandPool; }
	DkDevice& getDevice() { return m_device; }

	// Setters
	void setParameters(VkCommandPoolCreateFlags params);

	// Buffer allocation
	bool allocate(VkCommandBufferLevel level, uint count, std::vector<DkCommandBuffer*>& allocOut); // set of buffers
	bool allocate(std::vector<DkFrameResources*>& frames); // one for each in a set of frames
	DkCommandBuffer* allocate(VkCommandBufferLevel level); // single buffer
	void freeBuffer(DkCommandBuffer*& bfr);
private:
	// On construction
	DkDevice& m_device;
	DkQueue& m_queue;

	// Set before init
	VkCommandPoolCreateFlags m_parameters;

	// Set by init call
	VkCommandPool m_commandPool;
	bool m_initialized;

	// Tracked references
	std::vector<DkCommandBuffer*> m_allocatedBuffers;
};

#endif//DK_COMMAND_POOL_H
