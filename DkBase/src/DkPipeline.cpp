#include "DkPipeline.h"
#include "DkMesh.h"
#include "DkShader.h"
#include "DkDevice.h"

DkPipeline::DkPipeline(DkDevice& device, DkRenderPass& renderPass) :
	m_device(device),
	m_renderPass(renderPass),
	m_viewPortRect({ { 0u, 0u }, { 1280u, 960u } }),
	m_createFlags(0),
	m_shaders(),
	m_pushConstantRanges(),
	m_layoutBindings(),
	m_inVertBinds(),
	m_inVertAtts(),
	m_descriptorSetLayout(VK_NULL_HANDLE),
	m_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
	m_polygonMode(VK_POLYGON_MODE_FILL),
	m_cullMode(VK_CULL_MODE_NONE),
	m_frontFace(VK_FRONT_FACE_CLOCKWISE),
	m_depthTestEnabled(false),
	m_layout(VK_NULL_HANDLE),
	m_pipeline(VK_NULL_HANDLE),
	m_initialized(false),
	m_vertexBindingIndex(0)
{}

void DkPipeline::updateView(VkRect2D newView) {
	m_viewPortRect = newView;
}

void DkPipeline::setCreateFlags(VkPipelineCreateFlags flags) {
	if (m_initialized) {
		std::cout << "Cannot alter pipeline create flags after initialization." << std::endl;
		return;
	}
	m_createFlags = flags;
}

void DkPipeline::setTopology(VkPrimitiveTopology topology) {
	if (m_initialized) {
		std::cout << "Cannot alter primitive topology after initialization." << std::endl;
		return;
	}
	m_topology = topology;
}

void DkPipeline::setPolygonMode(VkPolygonMode mode) {
	if (m_initialized) {
		std::cout << "Cannot alter polygon mode after initialization." << std::endl;
		return;
	}
	m_polygonMode = mode;
}

void DkPipeline::setCullMode(VkCullModeFlags mode) {
	if (m_initialized) {
		std::cout << "Cannot alter cull mode after initialization." << std::endl;
		return;
	}
	m_cullMode = mode;
}

void DkPipeline::setFrontFace(VkFrontFace face) {
	if (m_initialized) {
		std::cout << "Cannot alter cull mode after initialization." << std::endl;
		return;
	}
	m_frontFace = face;
}

bool DkPipeline::addShader(const std::string& sourceFile, VkShaderStageFlagBits stage) {
	if (m_initialized) {
		std::cout << "Cannot add shader module after initialization." << std::endl;
		return false;
	}
	DkShader* toAdd = new DkShader(m_device, sourceFile, stage);
	if (!toAdd->init()) {
		delete toAdd;
		return false;
	}

	m_shaders.push_back(toAdd);
	return true;
}

void DkPipeline::addPushConstantRange(VkShaderStageFlags stage, uint offset, uint size) {
	if (m_initialized) {
		std::cout << "Cannot add push constant range after initialization." << std::endl;
		return;
	}
	m_pushConstantRanges.push_back({ stage, offset, size });
}

void DkPipeline::addDescriptorBinding(const VkDescriptorSetLayoutBinding& bndg) {
	if (m_initialized) {
		std::cout << "Cannot add descriptor binding after initialization." << std::endl;
		return;
	}
	for (auto& pre : m_layoutBindings) {
		if (bndg.binding == pre.binding) {
			std::cout << "Cannot add descriptor set layout binding of the same value as one previously added." << std::endl;
		}
	}
	m_layoutBindings.push_back(bndg);
}

void DkPipeline::setDepthTestEnabled(bool enabled) {
	if (m_initialized) {
		std::cout << "Cannot enable depth test after initialization." << std::endl;
		return;
	}
	m_depthTestEnabled = enabled;
}

bool DkPipeline::init() {
	std::vector<VkPipelineShaderStageCreateInfo> shaderInfo;
	VkPipelineShaderStageCreateInfo stageHold;
	for (auto& stage : m_shaders) {
		stage->getStageInfo(stageHold);
		shaderInfo.push_back(stageHold);
	}

	VkPipelineVertexInputStateCreateInfo vertexInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		(uint)m_inVertBinds.size(),
		m_inVertBinds.data(),
		(uint)m_inVertAtts.size(),
		m_inVertAtts.data()
	};

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		m_topology,
		VK_FALSE				// Primitive restart enable
	};

	VkViewport port = {
		(float)m_viewPortRect.offset.x, (float)m_viewPortRect.offset.y,
		(float)m_viewPortRect.extent.width, (float)m_viewPortRect.extent.height,
		0.f, 1.f				// min and max depth
	};

	VkPipelineViewportStateCreateInfo viewportInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,					// Viewport count
		&port,
		1,					// Scissor count (must equal viewport count)
		&m_viewPortRect
	};

	VkPipelineRasterizationStateCreateInfo rastInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,			// depth clamping
		VK_FALSE,			// rasterization discard
		m_polygonMode,
		m_cullMode,
		m_frontFace,
		VK_FALSE,			// depth bias enabled
		0.f,				// depth bias const factor
		0.f,				// depth bias clamp
		0.f,				// depth bias slope
		1.f					// line width
	};

	VkPipelineMultisampleStateCreateInfo multiSampInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_SAMPLE_COUNT_1_BIT,		// Sample count
		VK_FALSE,					// Sample shading enable
		0.f,						// Min sample shading
		nullptr,					// Sample mask
		VK_FALSE,					// Alpha to coverage enable
		VK_FALSE					// Alpha to one enable
	};

	VkStencilOpState dummyState = {};

	VkPipelineDepthStencilStateCreateInfo depthInfo;
	if (m_depthTestEnabled) {
		depthInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,						// depth test enabled
			VK_TRUE,						// depth writh enabled
			VK_COMPARE_OP_LESS_OR_EQUAL,	// depth compare op
			VK_FALSE,						// depth bounds test enable
			VK_FALSE,						// stencil test enable
			dummyState,						// stencil test front op
			dummyState,						// stencil test back op
			0.f,							// min depth bounds
			1.f								// max depth bounds
		};
	}

	VkPipelineColorBlendAttachmentState noBlend = {
		false,							// Color blend enable
		VK_BLEND_FACTOR_ONE,			// Src color blend factor
		VK_BLEND_FACTOR_ONE,			// Dst color blend factor
		VK_BLEND_OP_ADD,				// Color blend op
		VK_BLEND_FACTOR_ONE,			// Src alpha blend factor
		VK_BLEND_FACTOR_ONE,			// Dst alpha blend factor
		VK_BLEND_OP_ADD,				// Alpha blend op
		VK_COLOR_COMPONENT_A_BIT |		// Color write mask
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_R_BIT
	};

	VkPipelineColorBlendStateCreateInfo blendInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,					// Logic op enable
		VK_LOGIC_OP_NO_OP,			// Logic op
		1,							// Attachment count
		&noBlend,					// Blend attachment states
		{ 1.f, 1.f, 1.f, 1.f }		// Blend consts
	};

	std::vector<VkDynamicState> dynStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		(uint)dynStates.size(),
		dynStates.data()
	};

	if (m_layoutBindings.size() > 0) {
		VkDescriptorSetLayoutCreateInfo descSetInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			(uint)m_layoutBindings.size(),
			m_layoutBindings.data()
		};

		if (vkCreateDescriptorSetLayout(m_device.get(), &descSetInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			std::cout << "Failed to create descriptor set layout." << std::endl;
			return false;
		}
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		m_descriptorSetLayout == VK_NULL_HANDLE ? 0U : 1U,
		m_descriptorSetLayout == VK_NULL_HANDLE ? nullptr : &m_descriptorSetLayout,
		(uint)m_pushConstantRanges.size(),
		m_pushConstantRanges.size() == 0 ? nullptr : m_pushConstantRanges.data()
	};
	if (vkCreatePipelineLayout(m_device.get(), &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS || m_layout == VK_NULL_HANDLE) {
		std::cout << "Failed to create pipeline layout." << std::endl;
		return false;
	}

	VkGraphicsPipelineCreateInfo pipeInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		m_createFlags,
		(uint)shaderInfo.size(),
		shaderInfo.data(),
		&vertexInfo,
		&assemblyInfo,
		nullptr,									// Tessellation state
		&viewportInfo,
		&rastInfo,
		&multiSampInfo,
		m_depthTestEnabled ? &depthInfo : nullptr,
		&blendInfo,
		&dynamicInfo,
		m_layout,									// Pipeline layout-- pending implementation of nontrivial layouts
		m_renderPass.get(),
		0,											// Subpass index
		VK_NULL_HANDLE,								// Base pipeline handle
		0											// Base pipeline index
	};

	if (vkCreateGraphicsPipelines(m_device.get(), VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &m_pipeline) != VK_SUCCESS
		|| m_pipeline == VK_NULL_HANDLE) {
		std::cout << "Failed to create graphics pipeline." << std::endl;
		return false;
	}

	m_initialized = true;
	return true;
}

void DkPipeline::finalize() {
	for (auto& shader : m_shaders) {
		shader->finalize();
		delete shader;
		shader = nullptr;
	}
	m_shaders.clear();
	if (m_pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(m_device.get(), m_pipeline, nullptr);
		m_pipeline = VK_NULL_HANDLE;
	}
	if (m_layout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_device.get(), m_layout, nullptr);
		m_layout = VK_NULL_HANDLE;
	}
	if (m_descriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_device.get(), m_descriptorSetLayout, nullptr);
		m_descriptorSetLayout = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

VkPushConstantRange DkPipeline::getPushConstantRangeInfo(uint index) {
	if (index >= m_pushConstantRanges.size()) {
		std::cout << "Invalid push constant range index." << std::endl;
		return { 0, 0, 0 };
	}
	return m_pushConstantRanges[index];
}