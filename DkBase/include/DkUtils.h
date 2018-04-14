#ifndef DK_UTILS_H
#define DK_UTILS_H

#include "VulkanFunctions.h"

bool getAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableInstanceExtensions);
bool getAvailableDeviceExtensions(std::vector<VkExtensionProperties>& availableDeviceExtensions);
bool IsExtensionSupported(const std::vector<VkExtensionProperties>& available_extensions, const char* const extension);


#endif//DK_UTILS_H

