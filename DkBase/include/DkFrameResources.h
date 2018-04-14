#ifndef DK_FRAME_RESOURCES_H
#define DK_FRAME_RESOURCES_H

#include "DkCommon.h"
#include "DkSemaphore.h"
#include "DkFence.h"
#include "DkFramebuffer.h"
#include "DkImageView.h"

class DkDevice;
class DkCommandBuffer;
class DkCommandPool;
class DkSwapchain;

class DkFrameResources {
public:
	bool init();
	void finalize();
	
	// Setters
	void setCmdBfr(DkCommandBuffer* bfr) { m_cmdBfr = bfr; }

	// Getters
	DkCommandBuffer* getCmdBfr() { return m_cmdBfr; }
	uint getCurIndex() { return m_framebfr.getCurIndex(); }
	DkFramebuffer& getFramebuffer() { return m_framebfr; }
	DkSemaphore& getImgAcqSemaphore() { return m_imgAcqSemaphore; }
	DkSemaphore& getRdyPrsSemaphore() { return m_rdyPrsSemaphore; }
	DkFence& getFence() { return m_drawDoneFence; }

	bool reset();

	DkFrameResources(
		DkDevice& device,
		DkSwapchain* swapchain,
		DkRenderPass* renderPass
	);
	~DkFrameResources() { finalize(); }

private:
	bool _waitDrawDone();
	
	// Set on construction
	DkDevice& m_device;
	DkSwapchain* m_swapchain;

	// Set by init
	DkSemaphore m_imgAcqSemaphore;
	DkSemaphore m_rdyPrsSemaphore;
	DkFence m_drawDoneFence;
	DkImageView m_depthAttachment;

	// Set independently
	DkCommandBuffer* m_cmdBfr;

	// Managed internally
	DkFramebuffer m_framebfr;
};

#endif // !DK_FRAME_RESOURCES_H

