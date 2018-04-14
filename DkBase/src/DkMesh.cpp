#include "DkMesh.h"
#include "DkBuffer.h"

using namespace math;

DkMesh::DkMesh(DkBuffer* buffer) :
	m_bindIndex(0),
	m_buffer(buffer),
	m_extBuffer(buffer != nullptr),
	m_verts()
{}

void DkMesh::addVerts(const std::vector<vec3>& verts) {
	m_verts.insert(m_verts.end(), verts.begin(), verts.end());
}

void DkMesh::getPipelineCreateInfo(
	uint bindingIndex,
	std::vector<VkVertexInputBindingDescription>& bindingDescription,
	std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
) {
	bindingDescription.push_back({
		bindingIndex,
		sizeof(vec3),
		VK_VERTEX_INPUT_RATE_VERTEX
	});

	attributeDescriptions.push_back({
		0,
		bindingIndex,
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	});

	m_bindIndex = bindingIndex;
}

bool DkMesh::initBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue) {
	if (m_extBuffer) {
		std::cout << "Cannot init buffer; one has already been provided." << std::endl;
		return false;
	}

	if (m_buffer != nullptr) {
		std::cout << "Cannot init new buffer before finalizing current buffer." << std::endl;
	}

	m_buffer = new DkBuffer(device, nullptr);
	m_buffer->setSize(sizeof(vec3) * m_verts.size());
	m_buffer->setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	if (!m_buffer->init()) return false;
	if (!m_buffer->pushData((uint)m_buffer->getSize(), m_verts.data(), bfr, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, {}, queue)) return false;
	return true;
}

void DkMesh::finalizeBuffer() {
	if (!m_extBuffer && m_buffer != nullptr) {
		m_buffer->finalize();
		delete m_buffer;
		m_buffer = nullptr;
	}
}

void DkMesh::finalize() {
	finalizeBuffer();
	m_verts.clear();
}
