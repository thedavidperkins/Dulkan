#ifndef DK_QUEUE_H
#define DK_QUEUE_H

#include "DkCommon.h"

enum DkQueueType {
	DK_GRAPHICS_QUEUE = 0,
	DK_PRESENT_QUEUE = 1,
	DK_COMPUTE_QUEUE = 2,
	DK_NUM_QUEUE_TYPES = 3
};

class DkQueue {
public:
	// Getters
	DkQueueType getType() { return m_type; }
	uint getFamilyIndex() { return m_familyIndex; }
	VkQueue& get() { return m_queue; }

	// Setters
	void setType(DkQueueType type) { m_type = type; }
	void setQueueHandle(VkQueue handle) { m_queue = handle; }
	void setFamIndex(uint ind) { m_familyIndex = ind; }
private:
	DkQueueType m_type;
	VkQueue m_queue;
	uint m_familyIndex;
};

#endif//DK_QUEUE_H
