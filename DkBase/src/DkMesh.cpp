#include "DkMesh.h"
#include "DkBuffer.h"
#include "DkUniformBuffer.h"
#include "DkSemaphore.h"

using namespace math;

DkMesh::DkMesh(DkBuffer* buffer) :
	m_bindIndex(0),
	m_vertBuffer(buffer),
	m_mvpBuffer(nullptr),
	m_MVPs(),
	m_extBuffer(buffer != nullptr),
	m_verts()
{
	m_MVPs.resize(MAX_MESH_INSTANCES);
}

void DkMesh::addVerts(const std::vector<DkVertex>& verts) {
	m_verts.insert(m_verts.end(), verts.begin(), verts.end());
}

bool DkMesh::pushMVP(DkCommandBuffer* bfr, DkQueue& queue, const std::vector<DkSemaphore*>& signalSemaphores) {
	if (m_mvpBuffer == nullptr) {
		std::cout << "Cannot push view matrix. Buffers must be initialized first." << std::endl;
		return false;
	}
	std::vector<mat4> locMVPs;
	for (auto& mvp : m_MVPs) {
		locMVPs.push_back(transpose(mvp));
	}
	return m_mvpBuffer->pushData((uint)(sizeof(mat4) * locMVPs.size()), locMVPs.data(), bfr, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, VK_ACCESS_UNIFORM_READ_BIT, signalSemaphores, queue);
}

DkBuffer* DkMesh::getMVPBuffer() {
	return m_mvpBuffer;
}

void DkMesh::getPipelineCreateInfo(
	uint bindingIndex,
	std::vector<VkVertexInputBindingDescription>& bindingDescription,
	std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
) {
	bindingDescription.push_back({
		bindingIndex,
		sizeof(DkVertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	});

	attributeDescriptions.push_back({
		0,
		bindingIndex,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		0
	});

	attributeDescriptions.push_back({
		1,
		bindingIndex,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		offsetof(DkVertex, color)
	});

	m_bindIndex = bindingIndex;
}

bool DkMesh::initVertBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue) {
	if (m_extBuffer) {
		std::cout << "Cannot init buffer; one has already been provided." << std::endl;
		return false;
	}

	if (m_vertBuffer != nullptr) {
		std::cout << "Cannot init new buffer before finalizing current buffer." << std::endl;
	}

	m_vertBuffer = new DkBuffer(device, nullptr);
	m_vertBuffer->setSize(sizeof(DkVertex) * m_verts.size());
	m_vertBuffer->setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	if (!m_vertBuffer->init()) return false;
	if (!m_vertBuffer->pushData((uint)m_vertBuffer->getSize(), m_verts.data(), bfr, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, {}, queue)) return false;

	// initialize transformation matrix buffer
	m_mvpBuffer = new DkUniformBuffer(device, nullptr);
	m_mvpBuffer->setSize(sizeof(mat4) * MAX_MESH_INSTANCES);
	if (!m_mvpBuffer->init()) return false;
	return pushMVP(bfr, queue);
}

void DkMesh::finalizeBuffer() {
	if (!m_extBuffer && m_vertBuffer != nullptr) {
		m_vertBuffer->finalize();
		delete m_vertBuffer;
		m_vertBuffer = nullptr;
	}

	if (m_mvpBuffer != nullptr) {
		delete m_mvpBuffer;
		m_mvpBuffer = nullptr;
	}
}

void DkMesh::finalize() {
	finalizeBuffer();
	m_verts.clear();
}
