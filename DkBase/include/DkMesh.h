#ifndef DK_MESH_H
#define DK_MESH_H

#include "DkCommon.h"
#include "DkMath.h"

class DkBuffer;
class DkDevice;
class DkQueue;
class DkCommandBuffer;

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
	void addVerts(const std::vector<math::vec3>& verts);

	DkMesh(DkBuffer* buffer);
	~DkMesh() { finalize(); }
	DkMesh(const DkMesh& rhs) = delete;
	DkMesh& operator=(const DkMesh& rhs) = delete;
private:
	uint m_bindIndex;
	DkBuffer* m_buffer;
	bool m_extBuffer;
	std::vector<math::vec3> m_verts;
};

#endif//DK_MESH_H

