#ifndef DK_ATTACHMENT_DESCRIPTION_BUILDER_H
#define DK_ATTACHMENT_DESCRIPTION_BUILDER_H

#include "DkCommon.h"

class DkAttachmentDescriptionBuilder {
public:
	static VkAttachmentDescription basicColorOutputAttachment();
	static VkAttachmentDescription basicDepthAttachment();
};

#endif//DK_ATTACHMENT_DESCRIPTION_BUILDER_H

