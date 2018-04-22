#include "DkAttachmentDescriptionBuilder.h"

VkAttachmentDescription DkAttachmentDescriptionBuilder::basicColorOutputAttachment() {
	return {
		0,									// flags
		VK_FORMAT_B8G8R8A8_UNORM,			// format
		VK_SAMPLE_COUNT_1_BIT,				// sample count
		VK_ATTACHMENT_LOAD_OP_CLEAR,		// load op
		VK_ATTACHMENT_STORE_OP_STORE,		// store op
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,	// stencil load op
		VK_ATTACHMENT_STORE_OP_DONT_CARE,	// stencil clear op
		VK_IMAGE_LAYOUT_UNDEFINED,			// initial layout
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		// final layout
	};
}

VkAttachmentDescription DkAttachmentDescriptionBuilder::basicDepthAttachment() {
	return {
		0,													// flags
		VK_FORMAT_D32_SFLOAT,								// format
		VK_SAMPLE_COUNT_1_BIT,								// sample count
		VK_ATTACHMENT_LOAD_OP_CLEAR,						// load op
		VK_ATTACHMENT_STORE_OP_DONT_CARE,					// store op
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,					// stencil load op
		VK_ATTACHMENT_STORE_OP_DONT_CARE,					// stencil clear op
		VK_IMAGE_LAYOUT_UNDEFINED,							// initial layout
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// final layout
	};
}