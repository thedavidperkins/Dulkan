#ifndef DK_DESCRIPTOR_POOL_H
#define DK_DESCRIPTOR_POOL_H

#include "DkCommon.h"

class DkDevice;
class DkDescriptorSet;

class DkDescriptorPool {
public:
	bool init();
	void finalize();

	// Setters
	void setCreateFlags(VkDescriptorPoolCreateFlags flags);
	void setMaxSets(uint maxSets);
	void addToPoolSize(VkDescriptorType type, uint count);

	// Getters
	VkDescriptorPool get() { return m_pool; }
	DkDevice& getDevice() { return m_device; }

	// Set management
	DkDescriptorSet* allocate(VkDescriptorSetLayout layout);

	DkDescriptorPool(DkDevice& device);
	DkDescriptorPool(const DkDescriptorPool& rhs) = delete;
	DkDescriptorPool& operator=(const DkDescriptorPool& rhs) = delete;
	~DkDescriptorPool() { finalize(); }
private:
	// set on construction
	DkDevice& m_device;

	// set before init
	VkDescriptorPoolCreateFlags m_flags;
	uint m_maxSets;
	std::vector<VkDescriptorPoolSize> m_poolSizes;

	// set by init
	VkDescriptorPool m_pool;
	bool m_initialized;

	// managed sets
	std::vector<DkDescriptorSet*> m_sets;
};

#endif//DK_DESCRIPTOR_POOL_H
