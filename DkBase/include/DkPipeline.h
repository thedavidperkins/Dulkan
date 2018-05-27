#ifndef DK_PIPELINE_H
#define DK_PIPELINE_H

#include "DkCommon.h"
#include "DkRenderPass.h"

class DkShader;
class DkMesh;
class DkDevice;

class DkPipeline {
public:
	bool init();
	void finalize();

	// Setters

	// Update view merely updates the value held by the DkPipeline object. If
	//	want to dynamically update the view state on the actual Vulkan pipeline,
	//	you need to do so via command buffer
	void updateView(VkRect2D newView);

	void setCreateFlags(VkPipelineCreateFlags flags);
	void setTopology(VkPrimitiveTopology topology);
	void setPolygonMode(VkPolygonMode mode);
	void setCullMode(VkCullModeFlags mode);
	void setFrontFace(VkFrontFace face);
	void setDepthTestEnabled(bool enabled);

	bool addShader(const std::string& sourceFile, VkShaderStageFlagBits stage);

	template<class T>
	void addVertexInfo() {
		T::getPipelineCreateInfo(m_vertexBindingIndex++, m_inVertBinds, m_inVertAtts);
	}

	void addPushConstantRange(VkShaderStageFlags stage, uint offset, uint size);
	void addDescriptorBinding(const VkDescriptorSetLayoutBinding& bndg);

	// Getters
	VkPipeline& get() { return m_pipeline; }
	VkPipelineLayout getLayoutHandle() { return m_layout; }
	VkPushConstantRange getPushConstantRangeInfo(uint index);
	VkDescriptorSetLayout getDescriptorSetLayout() { return m_descriptorSetLayout; }

	DkPipeline(DkDevice& device, DkRenderPass& renderPass);
	virtual ~DkPipeline() { finalize(); }
	DkPipeline(const DkPipeline& rhs) = delete;
	DkPipeline& operator=(const DkPipeline& rhs) = delete;
private:
	// Set on construction
	DkDevice& m_device;
	DkRenderPass& m_renderPass;

	// Set before init
	VkRect2D m_viewPortRect;
	VkPipelineCreateFlags m_createFlags;
	std::vector<DkShader*> m_shaders;
	std::vector<VkPushConstantRange> m_pushConstantRanges;
	std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
	std::vector<VkVertexInputBindingDescription> m_inVertBinds;
	std::vector<VkVertexInputAttributeDescription> m_inVertAtts;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPrimitiveTopology m_topology;
	VkPolygonMode m_polygonMode;
	VkCullModeFlags m_cullMode;
	VkFrontFace m_frontFace;
	bool m_depthTestEnabled;

	// Set by init
	VkPipelineLayout m_layout;
	VkPipeline m_pipeline;
	bool m_initialized;

	// Incremented
	uint m_vertexBindingIndex;
};

#endif//DK_PIPELINE_H

