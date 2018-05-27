#include "DkMesh.h"
#include "DkBuffer.h"
#include "DkUniformBuffer.h"
#include "DkSemaphore.h"

using namespace math;

DkMesh::DkMesh(DkBuffer* buffer, uint maxInstances) :
	m_maxInstances(maxInstances),
	m_vertBuffer(buffer),
	m_mvpBuffer(nullptr),
	m_MV(),
	m_proj(),
	m_extBuffer(buffer != nullptr),
	m_verts()
{
	m_MV.resize(m_maxInstances, ident<4>());
	m_proj.resize(m_maxInstances, ident<4>());
}

void DkMesh::addVerts(const std::vector<DkVertex>& verts) {
	m_verts.insert(m_verts.end(), verts.begin(), verts.end());
}

void DkMesh::setMV(const mat4& mv, uint index) {
	if (index >= m_maxInstances) {
		std::cout << "MV index exceeds max limit." << std::endl;
		return;
	}
	m_MV[index] = mv;
}

void DkMesh::setProj(const mat4& proj, uint index) {
	if (index >= m_maxInstances) {
		std::cout << "Proj index exceeds max limit." << std::endl;
		return;
	}
	m_proj[index] = proj;
}

bool DkMesh::pushMVP(DkCommandBuffer* bfr, DkQueue& queue, const std::vector<DkSemaphore*>& mvpSignalSemaphores, const std::vector<DkSemaphore*>& normalSignalSemaphores) {
	if (m_mvpBuffer == nullptr) {
		std::cout << "Cannot push view matrix. Buffers must be initialized first." << std::endl;
		return false;
	}
	std::vector<mat4> locMVPS;
	std::vector<mat4> locMVPSTranspInv;
	for (uint iter = 0; iter < m_MV.size(); ++iter) {
		locMVPS.push_back(transpose(m_proj[iter] * m_MV[iter]));
		if (m_mvpBufferNormal != nullptr) {
			locMVPSTranspInv.push_back(transpose(mat4(transpose(inverse(mat3(m_MV[iter]))))));
		}
	}
	bool ret = m_mvpBuffer->pushData((uint)(sizeof(mat4) * locMVPS.size()), locMVPS.data(), bfr, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, VK_ACCESS_UNIFORM_READ_BIT, mvpSignalSemaphores, queue);

	if (ret && m_mvpBufferNormal != nullptr) {
		return m_mvpBufferNormal->pushData((uint)(sizeof(mat4) * locMVPSTranspInv.size()), locMVPSTranspInv.data(), bfr, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, VK_ACCESS_UNIFORM_READ_BIT, normalSignalSemaphores, queue);
	}

	return ret;
}

DkBuffer* DkMesh::getMVPBuffer() {
	return m_mvpBuffer;
}

DkBuffer* DkMesh::getMVNormalBuffer() {
	return m_mvpBufferNormal;
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

	attributeDescriptions.push_back({
		2,
		bindingIndex,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		offsetof(DkVertex, normal)
	});
}

bool DkMesh::initVertBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue, bool useUniformMVPBuffer) {
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
	if (useUniformMVPBuffer) {
		m_mvpBuffer = new DkUniformBuffer(device, nullptr);
		m_mvpBuffer->setSize(sizeof(mat4) * MAX_MESH_INSTANCES);
		if (!m_mvpBuffer->init()) return false;

		m_mvpBufferNormal = new DkUniformBuffer(device, nullptr);
		m_mvpBufferNormal->setSize(sizeof(mat4) * MAX_MESH_INSTANCES);
		if (!m_mvpBufferNormal->init()) return false;

		return pushMVP(bfr, queue);
	}
	return true;
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

	if (m_mvpBufferNormal != nullptr) {
		delete m_mvpBufferNormal;
		m_mvpBufferNormal = nullptr;
	}
}

void DkMesh::finalize() {
	finalizeBuffer();
	m_verts.clear();
}
