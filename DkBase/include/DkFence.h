#ifndef DK_FENCE_H
#define DK_FENCE_H

#include "DkCommon.h"
#include "DkDevice.h"

class DkFence {
public:
	bool init(bool signaled);
	void finalize();

	// Getters
	VkFence& get() { return m_fence; }

	// Use
	bool wait(uint timeout = 3000000000u);
	bool reset();

	DkFence(DkDevice& device);
	~DkFence() { finalize(); }
private:
	// On construction
	DkDevice& m_device;

	// Set by init
	VkFence m_fence;
	bool m_initialized;
};

#endif//DK_FENCE_H

