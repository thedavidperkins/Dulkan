#ifndef DK_RENDERPASS_H
#define DK_RENDERPASS_H

#include "DkCommon.h"

class DkDevice;

class DkRenderPass {
public:
	bool init();
	void finalize();

	// Pre init work
	void addAttachmentDescription(const VkAttachmentDescription& desc);
	void addAttachmentDescriptions(const std::vector<VkAttachmentDescription>& descs);

	void addSubpassDescription(const VkSubpassDescription& desc);
	void addSubpassDescriptions(const std::vector<VkSubpassDescription>& descs);

	void addSubpassDependency(const VkSubpassDependency& dep);
	void addSubpassDependencies(const std::vector<VkSubpassDependency>& deps);

	// Getters
	VkRenderPass& get() { return m_renderPass; }

	DkRenderPass(DkDevice& device);
	~DkRenderPass() { finalize(); }
	DkRenderPass(const DkRenderPass& rhs) = delete;
	DkRenderPass& operator=(const DkRenderPass& rhs) = delete;
private:
	// Set on construction
	DkDevice& m_device;

	// Set before init
	std::vector<VkAttachmentDescription> m_attachments;
	std::vector<VkSubpassDescription> m_subpasses;
	std::vector<VkSubpassDependency> m_subDeps;

	// Set by init
	VkRenderPass m_renderPass;
	bool m_initialized;
};

#endif // !DK_RENDERPASS_H

