#include "DkCommandBuffer.h"
#include "DkQueue.h"
#include "DkFence.h"
#include "DkRenderPass.h"
#include "DkFramebuffer.h"
#include "DkPipeline.h"
#include "DkMesh.h"
#include "DkDescriptorSet.h"

DkCommandBuffer::DkCommandBuffer(DkCommandPool& pool) :
	m_pool(pool),
	m_bufLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY),
	m_commandBuffer(VK_NULL_HANDLE),
	m_initialized(false),
	m_recording(false),
	m_inRenderPass(false),
	m_submitted(false)
{}

void DkCommandBuffer::setBufferHandle(VkCommandBuffer bfr) { 
	m_commandBuffer = bfr;
	m_initialized = bfr != VK_NULL_HANDLE;
}

void DkCommandBuffer::setBufferLevel(VkCommandBufferLevel level) {
	if (m_bufLevel == VK_COMMAND_BUFFER_LEVEL_SECONDARY) {
		std::cout << "Secondary command buffers not currently supported." << std::endl;
		return;
	}
	m_bufLevel = level;
}

void DkCommandBuffer::finalize() {
	if (m_commandBuffer != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(m_pool.getDevice().get(), m_pool.get(), 1, &m_commandBuffer);
		setBufferHandle(VK_NULL_HANDLE);
	}
}

bool DkCommandBuffer::beginRecording(VkCommandBufferUsageFlags usage) {
	if (!m_initialized) {
		std::cout << "Cannot begin recording without a valid command buffer handle." << std::endl;
	}

	if (m_recording) {
		std::cout << "Cannot begin recording operation. Already recording." << std::endl;
		return false;
	}

	VkCommandBufferBeginInfo begInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr					// Inheritance info. Currently secondary levels are not supported.
	};

	if (vkBeginCommandBuffer(m_commandBuffer, &begInfo) != VK_SUCCESS) {
		std::cout << "Failed to begin command recording operation." << std::endl;
		return false;
	}
	m_recording = true;
	return true;
}

bool DkCommandBuffer::pushConstants(DkPipeline& pipeline, uint index, const void* data) {
	if (!m_recording) {
		std::cout << "Cannot push constants: Command buffer recording not yet initiated." << std::endl;
		return false;
	}

	VkPushConstantRange range = pipeline.getPushConstantRangeInfo(index);
	if (range.stageFlags == 0) {
		std::cout << "An error occurred while getting push constant range info." << std::endl;
		return false;
	}

	vkCmdPushConstants(m_commandBuffer, pipeline.getLayoutHandle(), range.stageFlags, range.offset, range.size, data);
	return true;
}

bool DkCommandBuffer::setMemoryBarrier(
	VkPipelineStageFlags producingStage,
	VkPipelineStageFlags consumingStage,
	const std::vector<DkGlobalTransition>& globalBarriers,
	const std::vector<DkBufferTransition>& bufferBarriers,
	const std::vector<DkImageTransition>& imageBarriers,
	VkDependencyFlags depFlags
) {
	if (!m_recording) {
		std::cout << "Cannot record set memory barrier command: Command buffer recording not yet initiated." << std::endl;
		return false;
	}

	std::vector<VkMemoryBarrier> memBars;
	for (auto& bar : globalBarriers) {
		memBars.push_back({
			VK_STRUCTURE_TYPE_MEMORY_BARRIER,
			nullptr,
			bar.oldAccess,
			bar.newAccess
		});
	}

	std::vector<VkBufferMemoryBarrier> bufBars;
	for (auto& bar : bufferBarriers) {
		bufBars.push_back({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			bar.oldAccess,
			bar.newAccess,
			bar.oldQF,
			bar.newQF,
			bar.bfr->get(),
			0,
			VK_WHOLE_SIZE
		});
	}

	std::vector<VkImageMemoryBarrier> imgBars;
	for (auto& bar : imageBarriers) {
		imgBars.push_back({
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			bar.oldAccess,
			bar.newAccess,
			bar.oldLayout,
			bar.newLayout,
			bar.oldQF,
			bar.newQF,
			bar.img,
			{
				bar.aspect,
				0,
				VK_REMAINING_MIP_LEVELS,
				0,
				VK_REMAINING_ARRAY_LAYERS
			}
		});
	}

	vkCmdPipelineBarrier(m_commandBuffer, producingStage, consumingStage, depFlags,
		(uint)memBars.size(), memBars.data(),
		(uint)bufBars.size(), bufBars.data(),
		(uint)imgBars.size(), imgBars.data());

	return true;
}

// Copies whole size of source to offset 0 of buffer. Will add other options
//	as the need arises.
bool DkCommandBuffer::deviceMemCopy(DkBuffer& dest, DkBuffer& source) {
	if (!m_recording) {
		std::cout << "Cannot record device mem copy command: Command buffer recording not yet initiated." << std::endl;
		return false;
	}
	VkBufferCopy copy = {
		0,					// src offset
		0,					// dest offset
		source.getSize()	// size
	};
	vkCmdCopyBuffer(m_commandBuffer, source.get(), dest.get(), 1, &copy);
	return true;
}

bool DkCommandBuffer::beginRenderPass(
	DkRenderPass* renderPass,
	DkFramebuffer* framebuffer,
	const std::vector<VkClearValue>& clearVals
) {
	if (!m_recording) {
		std::cout << "Cannot begin render pass: Command buffer recording not yet initiated." << std::endl;
		return false;
	}

	if (m_inRenderPass) {
		std::cout << "Already in render pass. Cannot begin a new one." << std::endl;
		return false;
	}

	VkRenderPassBeginInfo info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		renderPass->get(),
		framebuffer->get(),
		{ { 0, 0 }, framebuffer->getSize() },
		(uint)clearVals.size(),
		clearVals.data()
	};

	vkCmdBeginRenderPass(m_commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

	m_inRenderPass = true;
	return true;
}

bool DkCommandBuffer::bindDescriptorSet(DkDescriptorSet* descriptorSet, DkPipeline* pipeline) {
	if (!m_recording) {
		std::cout << "Cannot bind descriptor set. Not yet recording." << std::endl;
		return false;
	}
	VkDescriptorSet set = descriptorSet->get();
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getLayoutHandle(), 0, 1, &set, 0, nullptr);
	return true;
}

bool DkCommandBuffer::bindPipeline(DkPipeline* pipeline) {
	if (!m_inRenderPass) {
		std::cout << "Cannot bind pipeline. Render pass not yet started or already ended." << std::endl;
		return false;
	}

	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get());
	return true;
}

bool DkCommandBuffer::setViewport(uint firstViewport, const std::vector<VkViewport>& viewports) {
	if (!m_inRenderPass) {
		std::cout << "Cannot set viewport state. Render pass not yet started or already ended." << std::endl;
		return false;
	}

	vkCmdSetViewport(m_commandBuffer, firstViewport, (uint)viewports.size(), viewports.data());
	return true;
}

bool DkCommandBuffer::setScissor(uint firstScissor, const std::vector<VkRect2D>& scissors) {
	if (!m_inRenderPass) {
		std::cout << "Cannot set scissor state. Render pass not yet started or already ended." << std::endl;
		return false;
	}

	vkCmdSetScissor(m_commandBuffer, firstScissor, (uint)scissors.size(), scissors.data());
	return true;
}

bool DkCommandBuffer::bindVertexBuffers(const std::vector<DkMesh*>& vertices) {
	if (!m_inRenderPass) {
		std::cout << "Cannot bind vertex buffers. Render pass not yet started or already ended." << std::endl;
		return false;
	}

	if (vertices.empty()) {
		std::cout << "Cannot bind vertex buffers. No buffers provided." << std::endl;
		return false;
	}

	std::vector<VkBuffer> bfrs = { vertices[0]->getVertBuffer()->get() };
	std::vector<VkDeviceSize> offsets = { 0 }; // Pending implementation: non-zero vertex buffer offsets
	if (vertices.size() > 1) {
		for (size_t iter = 1; iter < vertices.size(); ++iter) {
			if (vertices[iter]->getBindingIndex() != vertices[iter - 1]->getBindingIndex() + 1) {
				std::cout << "Error: cannot bind multiple non-contiguous vertex buffers in a single call." << std::endl;
				return false;
			}
			bfrs.push_back(vertices[iter]->getVertBuffer()->get());
			offsets.push_back(0);
		}
	}

	vkCmdBindVertexBuffers(m_commandBuffer, vertices[0]->getBindingIndex(), (uint)bfrs.size(), bfrs.data(), offsets.data());
	return true;
}

bool DkCommandBuffer::draw(DkMesh* mesh, uint nInstances) {
	if (!m_inRenderPass) {
		std::cout << "Cannot execute draw command. Render pass not yet started or already ended." << std::endl;
		return false;
	}

	vkCmdDraw(m_commandBuffer, mesh->getVertCount(), nInstances, 0, 0); // Pending implementation: varied starting vertices and instances
	return true;
}

bool DkCommandBuffer::endRenderPass() {
	if (!m_inRenderPass) {
		std::cout << "Cannot end render pass. Render pass not yet started or already ended." << std::endl;
		return false;
	}
	vkCmdEndRenderPass(m_commandBuffer);
	m_inRenderPass = false;
	return true;
}

bool DkCommandBuffer::endRecording() {
	if (!m_recording) {
		std::cout << "Cannot end recording. No recording operation has started." << std::endl;
		return false;
	}
	if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
		std::cout << "Failed to end command buffer recording operation." << std::endl;
		return false;
	}
	m_recording = false;
	return true;
}

bool DkCommandBuffer::submit(
	DkQueue& queue,
	const std::vector<DkWaitSemaphoreData>& waitSemaphores,
	const std::vector<DkSemaphore*>& signalSemaphores,
	DkFence& fence
) {
	if (!m_initialized) {
		std::cout << "Cannot submit: command buffer not yet initialized." << std::endl;
		return false;
	}
	
	if (m_recording) {
		std::cout << "Cannot submit: command recording not yet ended." << std::endl;
		return false;
	}
	
	std::vector<VkSemaphore> waitSems;
	std::vector<VkPipelineStageFlags> stages;
	for (auto& waitData : waitSemaphores) {
		waitSems.push_back(waitData.semaphore->get());
		stages.push_back(waitData.stage);
	}

	std::vector<VkSemaphore> sigSems;
	for (auto& semaphore : signalSemaphores) {
		sigSems.push_back(semaphore->get());
	}

	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		(uint)waitSems.size(),
		waitSems.data(),
		stages.data(),
		1,						// number of command buffers
		&m_commandBuffer,
		(uint)sigSems.size(),
		sigSems.data()
	};

	if (vkQueueSubmit(queue.get(), 1, &submitInfo, fence.get()) != VK_SUCCESS) {
		std::cout << "Failed to submit command buffer to queue." << std::endl;
		return false;
	}

	m_submitted = true;
	return true;
}