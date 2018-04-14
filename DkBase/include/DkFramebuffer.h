#ifndef DK_FRAMEBUFFER_H
#define DK_FRAMEBUFFER_H

#include "DkCommon.h"

class DkDevice;
class DkSwapchain;
class DkRenderPass;
class DkSemaphore;
class DkWindow;

class DkFramebuffer {
public:
	bool init();
	void finalize();
	bool reset();

	// Getters
	VkFramebuffer get() { return m_framebuffer; }
	uint getCurIndex() { return m_imgIndex; }
	VkExtent2D getSize() { return m_frameSize; }

	DkFramebuffer(
		DkDevice& device,
		DkSwapchain* swapchain,
		DkRenderPass* renderPass,
		DkSemaphore* imageAcquiredSemaphore
	);
	~DkFramebuffer() { finalize(); };
private:
	// Set on construction
	DkDevice& m_device;
	DkSwapchain* m_swapchain;
	DkRenderPass* m_renderPass;
	DkSemaphore* m_imageAcquiredSemaphore;
	DkWindow* m_window;

	// Set by init
	VkFramebuffer m_framebuffer;
	VkExtent2D m_frameSize;
	uint m_imgIndex;
	bool m_initialized;
};

#endif//DK_FRAMEBUFFER_H

