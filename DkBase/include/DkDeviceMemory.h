#ifndef DK_DEVICE_MEMORY_H
#define DK_DEVICE_MEMORY_H

#include "DkCommon.h"

class DkDevice;
class DkImage;
class DkBuffer;
struct DkBindLog;

enum DkResourceType {
	DK_IMAGE_RESOURCE = 0,
	DK_BUFFER_RESOURCE = 1,
	DK_RESOURCE_TYPE_COUNT = 2
};

class DkDeviceMemory {
public:
	// Note: either setOwner or setMemReqs MUST be called prior to init. This
	//	function will fail otherwise.
	bool init();
	void finalize();

	// Setters

	// Note: setMemReqs is intended for developers wishing to bypass exclusive
	//	ownership of a memory object by a buffer or image. Since with an owner
	//	we can determine the memory requirements ourselves, this function is
	//	only of use when no owner is set
	void setMemReqs(VkMemoryRequirements reqs);

	// Note: setOwner is intended to set an exclusive owner. Either an image or
	//	a buffer can own a memory object, but not both at the same time. On init
	//	the last owner set will be assumed to be the owner. This is only
	//	intended for use of images and buffers with exclusively allocated memory
	void setOwner(DkImage* owner);
	void setOwner(DkBuffer* owner);

	void setPropFlags(VkMemoryPropertyFlags flags);

	// Getters
	VkDeviceMemory& get() { return m_devMemory; }

	// Binding
	bool bind(DkImage* img, VkDeviceSize size);
	bool bind(DkBuffer* bfr, VkDeviceSize size);

	// Querying
	bool getMyOffsetAndSize(const void* resource, DkResourceType type, VkDeviceSize& offset, VkDeviceSize& size);

	DkDeviceMemory(DkDevice& device);
	~DkDeviceMemory() { finalize(); }
	DkDeviceMemory(const DkDeviceMemory& rhs) = delete;
	DkDeviceMemory& operator=(const DkDeviceMemory& rhs) = delete;
private:
	// Set by constructor
	DkDevice& m_device;
	
	// Set just one of these before init
	DkImage* m_img;
	DkBuffer* m_bfr;
	VkMemoryRequirements m_reqs;	

	// Set before init
	VkMemoryPropertyFlags m_memProps;

	// Set on init
	VkDeviceMemory m_devMemory;
	bool m_initialized;

	// Resource tracking
	VkDeviceSize m_offset;
	std::vector<DkBindLog> m_bindings;
};

union DkMemoryResource {
	DkImage* img;
	DkBuffer* bfr;
};

struct DkBindLog {
	VkDeviceSize start;
	VkDeviceSize size;
	DkResourceType type;
	DkMemoryResource resource;
};

#endif//DK_DEVICE_MEMORY_H

