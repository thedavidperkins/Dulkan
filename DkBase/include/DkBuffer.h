#ifndef DK_BUFFER_H
#define DK_BUFFER_H

#include "DkCommon.h"

class DkDevice;
class DkDeviceMemory;
class DkCommandBuffer;
class DkSemaphore;
class DkQueue;

class DkBuffer {
public:
	bool init();
	void finalize();

	// Setters
	void setCreateFlags(VkBufferCreateFlags flags);
	void setSize(VkDeviceSize size);
	virtual void setUsage(VkBufferUsageFlags usage);
	void setSharingMode(VkSharingMode mode);
	void setQueueFamilies(const std::vector<uint>& indices);

	// Getters
	VkBuffer get() { return m_buffer; }
	DkDeviceMemory* getMemory() { return m_memory; }
	VkDeviceSize getSize() { return m_queriedSize; }
	VkDeviceSize getOffset() { return m_queriedOffset; }

	// Access
	bool pushData(
		uint size,
		void* data,
		DkCommandBuffer* bfr,
		VkPipelineStageFlags producingStage,
		VkPipelineStageFlags consumingStage,
		VkAccessFlags oldAccess,
		VkAccessFlags newAccess,
		const std::vector<DkSemaphore*>& signalSemaphores,
		DkQueue& queue
	);

	DkBuffer(DkDevice& device, DkDeviceMemory* memory);
	~DkBuffer() { finalize(); }
private:
	// Set on construction
	DkDevice& m_device;
	DkDeviceMemory* m_memory;
	bool m_extMemory;

	// Set before init
	VkBufferCreateFlags m_createFlags;
	VkDeviceSize m_size;
	VkBufferUsageFlags m_usage;
	VkSharingMode m_sharingMode;
	std::vector<uint> m_qFamIndices;

	// Set by init
	VkBuffer m_buffer;
	bool m_initialized;
	VkDeviceSize m_queriedSize;
	VkDeviceSize m_queriedOffset;
};

#endif//DK_BUFFER_H

