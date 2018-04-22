#ifndef DK_MESH_H
#define DK_MESH_H

#include "DkCommon.h"
#include "DkMath.h"

class DkBuffer;
class DkDevice;
class DkQueue;
class DkCommandBuffer;

struct DkVertex {
	math::vec4 vert;
	math::vec4 color;
};

class DkMesh {
public:
	bool initBuffer(DkDevice& device, DkCommandBuffer* bfr, DkQueue& queue);
	void finalizeBuffer();
	void finalize();

	// Getters
	void getPipelineCreateInfo(
		uint bindingIndex,
		std::vector<VkVertexInputBindingDescription>& bindingDescription,
		std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
	);
	DkBuffer* getBuffer() { return m_buffer; }
	uint getBindingIndex() { return m_bindIndex; }
	uint getVertCount() { return (uint)m_verts.size(); }

	// Setters
	void addVerts(const std::vector<DkVertex>& verts);

	DkMesh(DkBuffer* buffer);
	~DkMesh() { finalize(); }
	DkMesh(const DkMesh& rhs) = delete;
	DkMesh& operator=(const DkMesh& rhs) = delete;
private:
	uint m_bindIndex;
	DkBuffer* m_buffer;
	bool m_extBuffer;
	std::vector<DkVertex> m_verts;
};

#endif//DK_MESH_H

