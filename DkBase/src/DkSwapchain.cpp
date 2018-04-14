#include "DkSwapchain.h"
#include "DkSemaphore.h"
#include "DkWindow.h"
#include "DkDevice.h"

DkSwapchain::DkSwapchain(DkDevice& device, DkWindow& window) :
	m_device(device),
	m_window(window),
	m_createFlags(0),
	m_presentMode(VK_PRESENT_MODE_MAILBOX_KHR),
	m_surfaceFormat({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR }),
	m_imgSurplus(1),
	m_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
	m_transform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
	m_imgCount(0),
	m_imgSize({}),
	m_swapchain(VK_NULL_HANDLE),
	m_images(),
	m_initialized(false)
{}

void DkSwapchain::setCreateFlags(VkSwapchainCreateFlagsKHR flags) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain create flags after initialization." << std::endl;
		return;
	}
	m_createFlags = flags;
}

void DkSwapchain::setPresentMode(VkPresentModeKHR mode) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain present mode after initialization." << std::endl;
		return;
	}
	m_presentMode = mode;
}

void DkSwapchain::setFormat(VkFormat format) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain format after initialization." << std::endl;
		return;
	}
	m_surfaceFormat.format = format;
}

void DkSwapchain::setColorSpace(VkColorSpaceKHR colorSpace) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain color space after initialization." << std::endl;
		return;
	}
	m_surfaceFormat.colorSpace = colorSpace;
}

void DkSwapchain::setImgSurplus(uint surplus) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain image count after initialization." << std::endl;
		return;
	}
	m_imgSurplus = surplus;
}

void DkSwapchain::setUsage(VkImageUsageFlags usage) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain image usage after initialization." << std::endl;
		return;
	}
	m_usage = usage;
}

void DkSwapchain::setTransform(VkSurfaceTransformFlagBitsKHR transform) {
	if (m_initialized) {
		std::cout << "Cannot alter swapchain image transform after initialization." << std::endl;
		return;
	}
	m_transform = transform;
}

bool DkSwapchain::init() {
	if (!m_device.waitIdle()) return false;

	VkSurfaceCapabilitiesKHR caps = m_window.getSurfaceCapabilities();
	if (!m_initialized) {
		// Confirm that we can use the requested present mode -- will set to basic FIFO if not
		m_window.checkPresentMode(m_presentMode);

		// Get swapchain image count
		m_imgCount = caps.minImageCount + m_imgSurplus;
		if ((caps.maxImageCount > 0) && (m_imgCount > caps.maxImageCount)) m_imgCount = caps.maxImageCount;

		// Double check image usage
		if ((m_usage & caps.supportedUsageFlags) != m_usage) {
			std::cout << "Requested usage flages not supported by surface." << std::endl;
			return false;
		}

		// Double check transform
		if ((caps.supportedTransforms & m_transform) != m_transform) {
			std::cout << "Requested transform not supported. Using current surface transform." << std::endl;
			m_transform = caps.currentTransform;
		}

		// Get nearest possible to desired format
		if (!m_window.getNearestAvailableSurfaceFormat(m_surfaceFormat)) return false;
	}
	
	// Get swapchain image size
	if (caps.currentExtent.width == 0xFFFFFFFF) {
		m_imgSize = m_window.getExtent();
		if (m_imgSize.width < caps.minImageExtent.width) {
			m_imgSize.width = caps.minImageExtent.width; 
		}
		else if (m_imgSize.width > caps.maxImageExtent.width) {
			m_imgSize.width = caps.maxImageExtent.width;
		}

		if (m_imgSize.height < caps.minImageExtent.height) {
			m_imgSize.height = caps.minImageExtent.height;
		}
		else if (m_imgSize.height > caps.maxImageExtent.height) {
			m_imgSize.height = caps.maxImageExtent.height;
		}
	}
	else {
		m_imgSize = caps.currentExtent;
	}

	m_window.setSurfaceExtent(m_imgSize);

	VkSwapchainKHR oldSwapchain = m_swapchain;

	VkSwapchainCreateInfoKHR swapchainInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		m_createFlags,
		m_window.getSurface(),
		m_imgCount,
		m_surfaceFormat.format,
		m_surfaceFormat.colorSpace,
		m_imgSize,
		1,									// array layers
		m_usage,
		VK_SHARING_MODE_EXCLUSIVE,			// sharing mode
		0,									// queue family count
		nullptr,							// queue family indices
		m_transform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,	// composite alpha
		m_presentMode,
		VK_TRUE,							// clipped
		oldSwapchain
	};

	if (vkCreateSwapchainKHR(m_device.get(), &swapchainInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
		std::cout << "Failed to create swapchain." << std::endl;
		return false;
	}

	if (oldSwapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_device.get(), oldSwapchain, nullptr);
		oldSwapchain = VK_NULL_HANDLE;
	}

	if (!_populateImageList()) return false;
	m_initialized = true;
	return true;
}

bool DkSwapchain::_populateImageList() {
	uint imgCount;
	if (vkGetSwapchainImagesKHR(m_device.get(), m_swapchain, &imgCount, nullptr) != VK_SUCCESS) {
		std::cout << "Failed to get swapchain image count." << std::endl;
		return false;
	}
	std::vector<VkImage> imgs;
	imgs.resize(imgCount);
	if (vkGetSwapchainImagesKHR(m_device.get(), m_swapchain, &imgCount, imgs.data()) != VK_SUCCESS) {
		std::cout << "Failed to get swapchain image list." << std::endl;
		return false;
	}

	_clearImageList();
	for (auto& img : imgs) {
		DkImageView* view = new DkImageView(m_device, nullptr);
		view->setImageHandle(img, m_surfaceFormat.format);
		if (!view->init()) {
			delete view;
			return false;
		}
		m_images.push_back({ img, view });
	}
	return true;
}

void DkSwapchain::_clearImageList() {
	for (auto& img : m_images) {
		if (img.View != nullptr) {
			delete img.View;
		}
	}
	m_images.clear();
}

void DkSwapchain::finalize() {
	_clearImageList();
	if (m_swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_device.get(), m_swapchain, nullptr);
		m_swapchain = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

DkImageView* DkSwapchain::getNextImg(DkSemaphore* imgAcquiredSemaphore, uint& imgIndex, uint timeout) {
	if (!m_initialized) {
		std::cout << "Cannot get image view. Swapchain not initialized." << std::endl;
		return nullptr;
	}
	if (imgAcquiredSemaphore == nullptr) {
		std::cout << "Cannot get image view. No valid semaphore provided." << std::endl;
	}
	VkResult res = vkAcquireNextImageKHR(m_device.get(), m_swapchain, timeout,
		imgAcquiredSemaphore->get(), VK_NULL_HANDLE, &imgIndex);
	if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		std::cout << "Failed to acquire image index." << std::endl;
		return nullptr;
	}

	return m_images[imgIndex].View;
}