#include "DkImageView.h"
#include "DkImage.h"
#include "DkDevice.h"

DkImageView::DkImageView(DkDevice& device, DkImage* image) :
	m_device(device),
	m_image(image),
	m_extImage(image != nullptr),
	m_imgHandle(VK_NULL_HANDLE),
	m_viewType(VK_IMAGE_VIEW_TYPE_2D),
	m_format(VK_FORMAT_B8G8R8A8_UNORM),
	m_compMapping({}),
	m_subRange({
		VK_IMAGE_ASPECT_COLOR_BIT,
		0,
		VK_REMAINING_MIP_LEVELS,
		0,
		VK_REMAINING_ARRAY_LAYERS
	}),
	m_directHandle(false),
	m_imageView(VK_NULL_HANDLE),
	m_initialized(false)
{}

void DkImageView::setImageParams(DkDevice& device, VkExtent3D extent, DkDeviceMemory* memory) {
	if (!m_initialized && !m_extImage && m_image == nullptr) {
		m_image = new DkImage(device, extent, memory);
	}
	else {
		std::cout << "Wasn't expecting setImageParams at this point." << std::endl;
	}
}

void DkImageView::setImageHandle(VkImage img, VkFormat format) {
	if (!m_initialized && !m_extImage) {
		m_imgHandle = img;
		m_format = format;
		m_extImage = true;
		m_directHandle = true;
	}
	else {
		std::cout << "Wasn't expecting setImageHandle at this point." << std::endl;
	}
}

void DkImageView::setViewType(VkImageViewType type) {
	if (m_initialized) {
		std::cout << "Cannot alter image view type after initialization." << std::endl;
		return;
	}
	m_viewType = type;
}

void DkImageView::setFormat(VkFormat format) {
	if (m_directHandle) {
		std::cout << "Cannot reset format for direct handle image views." << std::endl;
		return;
	}
	if (m_initialized) {
		std::cout << "Cannot alter image view format after initialization." << std::endl;
		return;
	}
	if (m_extImage && m_image->getFormat() != format) {
		std::cout << "Image view format must agree with external image format." << std::endl;
		return;
	}
	if (!m_extImage) {
		if (m_image == nullptr) {
			std::cout << "Cannot set format before calling setImageParams." << std::endl;
		}
		m_image->setFormat(format);
	}
	m_format = format;
}

void DkImageView::setCompMapping(VkComponentMapping compMapping) {
	if (m_initialized) {
		std::cout << "Cannot alter image view component mapping after initialization." << std::endl;
		return;
	}
	m_compMapping = compMapping;
}

void DkImageView::setSubRange(VkImageSubresourceRange subRange) {
	if (m_initialized) {
		std::cout << "Cannot alter image view subresource range after initialization." << std::endl;
		return;
	}
	m_subRange = subRange;
}

void DkImageView::setAspect(VkImageAspectFlags aspect) {
	if (m_initialized) {
		std::cout << "Cannot alter image view aspect after initialization." << std::endl;
		return;
	}
	m_subRange.aspectMask = aspect;
}

bool DkImageView::init() {
	if (m_image == nullptr && m_imgHandle == VK_NULL_HANDLE) {
		std::cout << "Failed to create an image view: no image or image parameters provided." << std::endl;
	}

	if (!m_extImage && !m_image->init()) {
		return false;
	}

	if (m_image != nullptr) {
		m_imgHandle = m_image->get();
	}

	VkImageViewCreateInfo viewInfo = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		m_imgHandle,
		m_viewType,
		m_format,
		m_compMapping,
		m_subRange
	};

	VkResult res = vkCreateImageView(m_device.get(), &viewInfo, nullptr, &m_imageView);
	if (res != VK_SUCCESS) {
		std::cout << "Failed to create image view." << std::endl;
		return false;
	}
	
	m_initialized = true;
	return true;
}

void DkImageView::finalize() {
	if (!m_extImage && m_image != nullptr) {
		m_image->finalize();
		delete m_image;
		m_image = nullptr;
	}
	if (m_imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(m_device.get(), m_imageView, nullptr);
		m_imageView = VK_NULL_HANDLE;
	}
	m_initialized = false;
}