#ifndef DK_SWAPCHAIN_H
#define DK_SWAPCHAIN_H

#include "DkCommon.h"
#include "DkImageView.h"
#include "DkDeviceMemory.h"

class DkDevice;
class DkWindow;
class DkSemaphore;

struct SwapchainComponent {
	VkImage Img;
	DkImageView* View;
};

class DkSwapchain {
public:
	bool init();
	void finalize();
	bool resize();

	// Getters
	VkSwapchainKHR get() { return m_swapchain; }
	VkImage getImage(uint index) { return m_images[index].Img; }
	DkImageView* getNextImg(DkSemaphore* imgAcquiredSemaphore, uint& imgIndex, uint timeout = 500000000);
	VkExtent2D getImgSize() { return m_imgSize; }

	// Setters
	void setCreateFlags(VkSwapchainCreateFlagsKHR flags);
	void setPresentMode(VkPresentModeKHR mode);
	void setFormat(VkFormat format);
	void setColorSpace(VkColorSpaceKHR colorSpace);
	void setImgSurplus(uint surplus);
	void setUsage(VkImageUsageFlags usage);
	void setTransform(VkSurfaceTransformFlagBitsKHR transform);

	DkSwapchain(DkDevice& device, DkWindow& window);
	~DkSwapchain() { finalize(); }
	DkSwapchain(const DkSwapchain& rhs) = delete;
	DkSwapchain& operator=(const DkSwapchain& rhs) = delete;
private:
	// Helper functions
	bool _populateImageList();
	void _clearImageList();

	// Set on construction
	DkDevice& m_device;
	DkWindow& m_window;
	
	// Set before init
	VkSwapchainCreateFlagsKHR m_createFlags;
	VkPresentModeKHR m_presentMode;
	VkSurfaceFormatKHR m_surfaceFormat;
	uint m_imgSurplus; // how many images above the minimum for the presentation mode do we want?
	VkImageUsageFlags m_usage;
	VkSurfaceTransformFlagBitsKHR m_transform;

	// Set by init
	uint m_imgCount;
	VkExtent2D m_imgSize;
	VkSwapchainKHR m_swapchain;
	std::vector<SwapchainComponent> m_images;
	bool m_initialized;
};

#endif//DK_SWAPCHAIN_H
