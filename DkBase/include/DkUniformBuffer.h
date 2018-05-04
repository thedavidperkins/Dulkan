#ifndef DK_UNIFORM_BUFFER_H
#define DK_UNIFORM_BUFFER_H

#include "DkBuffer.h"
#include "DkDeviceMemory.h"
#include "DkDevice.h"

class DkUniformBuffer : public DkBuffer {
public:
	virtual void setUsage(VkBufferUsageFlags usage) {
		std::cout << "Cannot alter usage of DkUniformBuffer type" << std::endl;
	}

	DkUniformBuffer(DkDevice& device, DkDeviceMemory* memory) :
		DkBuffer(device, memory) 
	{
		DkBuffer::setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}

	~DkUniformBuffer() {}
};

#endif//DK_UNIFORM_BUFFER_H

