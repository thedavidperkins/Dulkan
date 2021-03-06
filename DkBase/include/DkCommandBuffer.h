#ifndef DK_COMMAND_BUFFER_H
#define DK_COMMAND_BUFFER_H

#include "DkCommon.h"
#include "DkCommandPool.h"
#include "DkBuffer.h"
#include "DkImage.h"
#include "DkSemaphore.h"

class DkQueue;
class DkFence;
class DkRenderPass;
class DkFramebuffer;
class DkPipeline;
class DkMesh;
class DkDescriptorSet;

struct DkBufferTransition {
	DkBuffer* bfr;
	VkAccessFlags oldAccess;
	VkAccessFlags newAccess;
	uint oldQF;
	uint newQF;
};

struct DkImageTransition {
	VkImage img;
	VkAccessFlags oldAccess;
	VkAccessFlags newAccess;
	VkImageLayout oldLayout;
	VkImageLayout newLayout;
	uint oldQF;
	uint newQF;
	VkImageAspectFlags aspect;
};

struct DkGlobalTransition {
	VkAccessFlags oldAccess;
	VkAccessFlags newAccess;
};

class DkCommandBuffer {
public:
	void finalize();

	// Getters
	VkCommandBuffer& get() { return m_commandBuffer; }
	VkCommandBufferLevel getLevel() { return m_bufLevel; }
	bool isRecording() { return m_recording; }

	void setBufferLevel(VkCommandBufferLevel level);
	
	// Setter of main buffer (called by allocate functions in DkCommandPool)
	void setBufferHandle(VkCommandBuffer bfr);

	// Sending commands
	bool beginRecording(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	bool pushConstants(DkPipeline& pipeline, uint index, const void* data);
	bool setMemoryBarrier(
		VkPipelineStageFlags producingStage,
		VkPipelineStageFlags consumingStage,
		const std::vector<DkGlobalTransition>& globalBarriers,
		const std::vector<DkBufferTransition>& bufferBarriers,
		const std::vector<DkImageTransition>& imageBarriers,
		VkDependencyFlags depFlags = 0
	);
	bool deviceMemCopy(DkBuffer& dest, DkBuffer& source);
	bool beginRenderPass(
		DkRenderPass* renderPass,
		DkFramebuffer* framebuffer,
		const std::vector<VkClearValue>& clearVals
	);
	bool bindDescriptorSet(DkDescriptorSet* descriptorSet, DkPipeline* pipeline);
	bool bindPipeline(DkPipeline* pipeline);
	bool setViewport(uint firstViewport, const std::vector<VkViewport>& viewports);
	bool setScissor(uint firstScissor, const std::vector<VkRect2D>& scissors);
	bool bindVertexBuffer(DkMesh* vertices);
	bool draw(DkMesh* mesh, uint nInstances = 1);
	bool endRenderPass();
	bool endRecording();
	bool submit(
		DkQueue& queue,
		const std::vector<DkWaitSemaphoreData>& waitSemaphores,
		const std::vector<DkSemaphore*>& signalSemaphores,
		DkFence& fence
	);

	DkCommandBuffer(DkCommandPool& pool);
	~DkCommandBuffer() { finalize(); }
	DkCommandBuffer(const DkCommandBuffer& rhs) = delete;
	DkCommandBuffer& operator=(const DkCommandBuffer& rhs) = delete;
private:
	// On construction
	DkCommandPool& m_pool;

	// Set before init
	VkCommandBufferLevel m_bufLevel;

	// Set by allocate (external by command pool)
	VkCommandBuffer m_commandBuffer;

	// State monitor
	bool m_initialized;
	bool m_recording;
	bool m_inRenderPass;
	bool m_submitted;
};



#endif//DK_COMMAND_BUFFER_H

