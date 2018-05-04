#ifndef DK_SEMAPHORE_H
#define DK_SEMAPHORE_H

#include "DkCommon.h"
#include "DkDevice.h"

class DkSemaphore {
public:
	bool init();
	void finalize();

	// Getters
	VkSemaphore get() const { return m_semaphore; }

	//bool wait();

	DkSemaphore(DkDevice& device);
	~DkSemaphore() { finalize(); }
private:
	// On construction
	DkDevice& m_device;
	
	// Set by init
	VkSemaphore m_semaphore;
	bool m_initialized;
};

struct DkWaitSemaphoreData {
	DkSemaphore* semaphore;
	VkPipelineStageFlags stage;
};

#endif//DK_SEMAPHORE_H

