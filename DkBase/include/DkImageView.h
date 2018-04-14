#ifndef DK_IMAGE_VIEW_H
#define DK_IMAGE_VIEW_H

#include "DkCommon.h"

class DkImage;
class DkDevice;
class DkDeviceMemory;

class DkImageView {
public:
	bool init();
	void finalize();

	// Call if image is not provided on construction or by direct handle
	void setImageParams(DkDevice& device, VkExtent3D extent, DkDeviceMemory* memory);

	// Call where it is not practical to wrap VkImage in DkImage, as with swapchain images
	void setImageHandle(VkImage img, VkFormat format);

	// Setters
	void setViewType(VkImageViewType type);
	void setFormat(VkFormat format);
	void setCompMapping(VkComponentMapping compMapping);
	void setSubRange(VkImageSubresourceRange subRange);
	void setAspect(VkImageAspectFlags aspect);

	// Getters
	VkImageView& get() { return m_imageView; }
	DkImage* getImage() { return m_image; }
	VkImage getImgHandle() { return m_imgHandle; }

	DkImageView(DkDevice& device, DkImage* image);
	~DkImageView() { finalize(); }
	DkImageView(const DkImageView& rhs) = delete;
	DkImageView& operator=(const DkImageView& rhs) = delete;
private:
	// Set on construction
	DkDevice& m_device;
	DkImage* m_image;
	bool m_extImage;
	VkImage m_imgHandle;

	// Set before init
	VkImageViewType m_viewType;
	VkFormat m_format;
	VkComponentMapping m_compMapping;
	VkImageSubresourceRange m_subRange;
	bool m_directHandle;

	// Set by init
	VkImageView m_imageView;
	bool m_initialized;
};

#endif//DK_IMAGE_VIEW_H
