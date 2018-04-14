#ifndef DK_IMAGE_H
#define DK_IMAGE_H

#include "DkCommon.h"
#include "DkDeviceMemory.h"

class DkDevice;

/*
*	class DkImage:
*
*	Manages resources and state of VkImage objects. VkImages can be initialized
*	with a variety of options, so there are quite a few pre-init setters. Most
*	can be left to their default values from construction (see implementation),
*	but developers should probably always call setUsage() and setFormat() prior
*	to calling init()
*
*/
class DkImage {
public:
	bool init();
	void finalize();

	// Getters
	VkImage get() { return m_image; }
	DkDeviceMemory* getMemory() { return m_memory; }
	DkDevice& getDevice() { return m_device; }
	VkFormat getFormat() { return m_format; }
	VkDeviceSize getSize() { return m_queriedSize; }
	VkDeviceSize getOffset() { return m_queriedOffset; }

	// Setters
	void setCreateFlags(VkImageCreateFlags flags);
	void setImgType(VkImageType type);
	void setFormat(VkFormat format);
	void setMipLevels(uint levels);
	void setArrLayers(uint layers);
	void setSampCount(VkSampleCountFlagBits sampCount);
	void setTiling(VkImageTiling tiling);
	void setUsage(VkImageUsageFlags usage);
	void setSharing(VkSharingMode sharing);
	void setQueueFamilies(const std::vector<uint>& indices);

	DkImage(DkDevice& device, VkExtent3D extent, DkDeviceMemory* memory);
	~DkImage() { finalize(); }
	DkImage(const DkImage& rhs) = delete;
	DkImage& operator=(const DkImage& rhs) = delete;
private:
	// Set on construction
	DkDevice& m_device;
	VkExtent3D m_extent;
	DkDeviceMemory* m_memory;
	bool m_extMemory;

	// Set before init
	VkImageCreateFlags m_imgCreateFlags;
	VkImageType m_imgType;
	VkFormat m_format;
	uint m_mipLevels;
	uint m_arrLayers;
	VkSampleCountFlagBits m_sampCount;
	VkImageTiling m_tiling;
	VkImageUsageFlags m_usage;
	VkSharingMode m_sharing;
	std::vector<uint> m_qFamIndices;

	// Set by init
	VkImage m_image;
	bool m_initialized;
	VkDeviceSize m_queriedSize;
	VkDeviceSize m_queriedOffset;
};

#endif//DK_IMAGE_H