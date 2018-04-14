#include "DkRenderPass.h"
#include "DkDevice.h"

DkRenderPass::DkRenderPass(DkDevice& device) :
	m_device(device),
	m_attachments(),
	m_subpasses(),
	m_subDeps(),
	m_renderPass(VK_NULL_HANDLE),
	m_initialized(false)
{}

void DkRenderPass::addAttachmentDescription(const VkAttachmentDescription& desc) {
	if (m_initialized) {
		std::cout << "Cannot add attachment descriptions to render pass after initialization." << std::endl;
		return;
	}
	m_attachments.push_back(desc);
}

void DkRenderPass::addAttachmentDescriptions(const std::vector<VkAttachmentDescription>& descs) {
	if (m_initialized) {
		std::cout << "Cannot add attachment descriptions to render pass after initialization." << std::endl;
		return;
	}
	m_attachments.insert(m_attachments.end(), descs.begin(), descs.end());
}

void DkRenderPass::addSubpassDescription(const VkSubpassDescription& desc) {
	if (m_initialized) {
		std::cout << "Cannot add subpass descriptions to render pass after initialization." << std::endl;
		return;
	}
	m_subpasses.push_back(desc);
}

void DkRenderPass::addSubpassDescriptions(const std::vector<VkSubpassDescription>& descs) {
	if (m_initialized) {
		std::cout << "Cannot add subpass descriptions to render pass after initialization." << std::endl;
		return;
	}
	m_subpasses.insert(m_subpasses.end(), descs.begin(), descs.end());
}

void DkRenderPass::addSubpassDependency(const VkSubpassDependency& dep) {
	if (m_initialized) {
		std::cout << "Cannot add subpass dependencies to render pass after initialization." << std::endl;
		return;
	}
	m_subDeps.push_back(dep);
}

void DkRenderPass::addSubpassDependencies(const std::vector<VkSubpassDependency>& deps) {
	if (m_initialized) {
		std::cout << "Cannot add subpass dependencies to render pass after initialization." << std::endl;
		return;
	}
	m_subDeps.insert(m_subDeps.end(), deps.begin(), deps.end());
}



bool DkRenderPass::init() {
	VkRenderPassCreateInfo passInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		(uint)m_attachments.size(),
		m_attachments.data(),
		(uint)m_subpasses.size(),
		m_subpasses.data(),
		(uint)m_subDeps.size(),
		m_subDeps.data()
	};

	if (vkCreateRenderPass(m_device.get(), &passInfo, nullptr, &m_renderPass) != VK_SUCCESS || m_renderPass == VK_NULL_HANDLE) {
		std::cout << "Failed to create render pass" << std::endl;
		return false;
	}

	m_initialized = true;
	return true;
}

void DkRenderPass::finalize() {
	if (m_renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_device.get(), m_renderPass, nullptr);
		m_renderPass = VK_NULL_HANDLE;
	}
	m_initialized = false;
}