#ifndef DK_MESH_H
#define DK_MESH_H

#include "DkCommon.h"
#include "DkMath.h"

class DkBuffer;
class DkDevice;
class DkQueue;
class DkCommandBuffer;
class DkUniformBuffer;
class DkSemaphore;

const uint MAX_MESH_INSTANCES = 16;

struct DkVertex {
	math::vec4 vert;
	math::vec4 color;
	math::vec4 normal;
};

class DkMesh {
public:
	bool initVertBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue, bool useUniformMVPBuffer = true);
	void finalizeBuffer();
	void finalize();

	// Getters
	static void getPipelineCreateInfo(
		uint bindingIndex,
		std::vector<VkVertexInputBindingDescription>& bindingDescription,
		std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
	);
	DkBuffer* getVertBuffer() { return m_vertBuffer; }
	DkBuffer* getMVPBuffer();
	DkBuffer* getMVNormalBuffer();
	math::mat4 getMVP(uint index = 0) { return m_proj[index] * m_MV[index]; }
	uint getVertCount() { return (uint)m_verts.size(); }

	// Setters
	void addVerts(const std::vector<DkVertex>& verts);
	void setMV(const math::mat4& mv, uint index = 0);
	void setProj(const math::mat4& proj, uint index = 0);

	bool pushMVP(DkCommandBuffer* bfr, DkQueue& queue, const std::vector<DkSemaphore*>& mvpSignalSemaphores = {}, const std::vector<DkSemaphore*>& normalSignalSemaphores = {});

	DkMesh(DkBuffer* buffer, uint maxInstances = MAX_MESH_INSTANCES);
	~DkMesh() { finalize(); }
	DkMesh(const DkMesh& rhs) = delete;
	DkMesh& operator=(const DkMesh& rhs) = delete;
private:
	uint m_maxInstances;
	DkBuffer* m_vertBuffer;
	DkUniformBuffer* m_mvpBuffer;
	DkUniformBuffer* m_mvpBufferNormal;
	std::vector<math::mat4> m_MV;
	std::vector<math::mat4> m_proj;
	bool m_extBuffer;
	std::vector<DkVertex> m_verts;
};

#endif//DK_MESH_H

