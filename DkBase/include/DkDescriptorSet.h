#ifndef DK_DESCRIPTOR_SET_H
#define DK_DESCRIPTOR_SET_H

#include "DkCommon.h"

class DkDescriptorPool;
class DkBuffer;

class DkDescriptorSet {
public:
	void finalize();

	// Setters
	void setDescriptorSetHandle(VkDescriptorSet set);

	// Getters
	VkDescriptorSet get() { return m_set; }

	// Manipulate
	bool updateBuffer(
		uint binding,
		DkBuffer* buffer,
		VkDeviceSize offset,
		VkDeviceSize range,
		VkDescriptorType bufferType
	);

	DkDescriptorSet(DkDescriptorPool& pool);
	~DkDescriptorSet() { finalize(); }
	DkDescriptorSet(const DkDescriptorSet& rhs) = delete;
	DkDescriptorSet& operator=(const DkDescriptorSet& rhs) = delete;
private:
	// set on construction
	DkDescriptorPool& m_pool;

	// set by allocate (external by descriptor pool)
	VkDescriptorSet m_set;

	// state monitor
	bool m_initialized;
};

#endif//DK_DESCRIPTOR_SET_H

