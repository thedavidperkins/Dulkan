#include "DkFrameResources.h"
#include "DkDevice.h"
#include "DkCommandBuffer.h"
#include "DkImage.h"
#include "DkSwapchain.h"

DkFrameResources::DkFrameResources(
	DkDevice& device,
	DkSwapchain* swapchain,
	DkRenderPass* renderPass,
	bool useDepth
) :
	m_device(device),
	m_swapchain(swapchain),
	m_useDepth(useDepth),
	m_imgAcqSemaphore(device),
	m_rdyPrsSemaphore(device),
	m_drawDoneFence(device),
	m_depthAttachment(m_device, nullptr),
	m_cmdBfr(nullptr),
	m_framebfr(device, swapchain, renderPass, &m_imgAcqSemaphore, useDepth ? &m_depthAttachment : nullptr),
	m_initialized(false)
{}

bool DkFrameResources::init() {
	if (m_useDepth) {
		if (!_initDepthAttachment()) return false;
	}
	if (!m_imgAcqSemaphore.init(false)) return false;
	if (!m_rdyPrsSemaphore.init(false)) return false;
	if (!m_drawDoneFence.init(true)) return false; // expected usage is to wait for signal at start of loop, so start signaled for beginning

	m_initialized = true;
	return true;
}

bool DkFrameResources::_initDepthAttachment() {
	m_depthAttachment.setImageParams(m_device, { m_swapchain->getImgSize().width, m_swapchain->getImgSize().height, 1 }, nullptr);
	m_depthAttachment.setFormat(VK_FORMAT_D32_SFLOAT);
	m_depthAttachment.getImage()->setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	m_depthAttachment.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT);
	return m_depthAttachment.init();
}

void DkFrameResources::finalize() {
	m_cmdBfr = nullptr;
	m_imgAcqSemaphore.finalize();
	m_rdyPrsSemaphore.finalize();
	m_drawDoneFence.finalize();
	m_framebfr.finalize();
	m_depthAttachment.finalize();
	m_initialized = false;
}

bool DkFrameResources::_waitDrawDone() {
	return m_drawDoneFence.wait();
}

bool DkFrameResources::reset() {
	if (!_waitDrawDone()) return false;
	if (!m_drawDoneFence.reset()) return false;
	if (!m_framebfr.reset()) return false;
	return true;
}

bool DkFrameResources::resize() {
	if (m_useDepth) {
		if (!_waitDrawDone()) return false;
		m_depthAttachment.finalize();
		if (!_initDepthAttachment()) return false;
	}
	return true;
}