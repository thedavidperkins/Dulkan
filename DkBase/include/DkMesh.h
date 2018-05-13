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
};

class DkMesh {
public:
	bool initVertBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue);
	void finalizeBuffer();
	void finalize();

	// Getters
	void getPipelineCreateInfo(
		uint bindingIndex,
		std::vector<VkVertexInputBindingDescription>& bindingDescription,
		std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
	);
	DkBuffer* getVertBuffer() { return m_vertBuffer; }
	DkBuffer* getMVPBuffer();
	uint getBindingIndex() { return m_bindIndex; }
	uint getVertCount() { return (uint)m_verts.size(); }

	// Setters
	void addVerts(const std::vector<DkVertex>& verts);
	void setMVP(const math::mat4& mvp, uint index = 0) { m_MVPs[index] = mvp; }

	bool pushMVP(DkCommandBuffer* bfr, DkQueue& queue, const std::vector<DkSemaphore*>& signalSemaphores = {});

	DkMesh(DkBuffer* buffer);
	~DkMesh() { finalize(); }
	DkMesh(const DkMesh& rhs) = delete;
	DkMesh& operator=(const DkMesh& rhs) = delete;
private:
	uint m_bindIndex;
	DkBuffer* m_vertBuffer;
	DkUniformBuffer* m_mvpBuffer;
	std::vector<math::mat4> m_MVPs;
	bool m_extBuffer;
	std::vector<DkVertex> m_verts;
};

#endif//DK_MESH_H

