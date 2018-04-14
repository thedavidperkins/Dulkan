#include "DkFramebuffer.h"
#include "DkDevice.h"
#include "DkSwapchain.h"
#include "DkRenderPass.h"
#include "DkWindow.h"

DkFramebuffer::DkFramebuffer(
	DkDevice& device,
	DkSwapchain* swapchain,
	DkRenderPass* renderPass,
	DkSemaphore* imageAcquiredSemaphore
) :
	m_device(device),
	m_swapchain(swapchain),
	m_renderPass(renderPass),
	m_imageAcquiredSemaphore(imageAcquiredSemaphore),
	m_framebuffer(VK_NULL_HANDLE),
	m_imgIndex(0),
	m_initialized(false)
{}

bool DkFramebuffer::init() {
	if (m_swapchain == nullptr || m_renderPass == nullptr || m_imageAcquiredSemaphore == nullptr) {
		std::cout << "Invalid framebuffer initialization." << std::endl;
		return false;
	}

	DkImageView* attachment = m_swapchain->getNextImg(m_imageAcquiredSemaphore, m_imgIndex);

	if (attachment == nullptr) return false;

	m_frameSize = m_swapchain->getImgSize();

	VkFramebufferCreateInfo fbInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		m_renderPass->get(),
		1,							// number of attachments
		&attachment->get(),
		m_frameSize.width,
		m_frameSize.height,
		1							// layers
	};

	if (vkCreateFramebuffer(m_device.get(), &fbInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
		std::cout << "Failed to create framebuffer." << std::endl;
		return false;
	}

	m_initialized = true;
	return true;
}

void DkFramebuffer::finalize() {
	if (m_framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_device.get(), m_framebuffer, nullptr);
		m_framebuffer = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

bool DkFramebuffer::reset() {
	finalize();
	return init();
}