#include "DkUtils.h"
#include "DkCommon.h"
#include "DkApplication.h"
#include "DkPhysicalDevice.h"

bool getAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableInstanceExtensions) {
	uint extensions_count;
	availableInstanceExtensions.clear();
	VkResult result = VK_SUCCESS;
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
	if (result != VK_SUCCESS || extensions_count == 0) {
		std::cout << "Could not get the number of instance extensions." << std::endl;
		return false;
	}
	availableInstanceExtensions.resize(extensions_count);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, availableInstanceExtensions.data());
	if (result != VK_SUCCESS || extensions_count == 0) {
		std::cout << "Could not enumerate instance extensions." << std::endl;
		return false;
	}
	return true;
}

bool getAvailableDeviceExtensions(DkPhysicalDevice& physDevice, std::vector<VkExtensionProperties>& availableDeviceExtensions) {
	uint extensions_count;
	availableDeviceExtensions.clear();
	VkResult result = VK_SUCCESS;
	result = vkEnumerateDeviceExtensionProperties(physDevice.get(), nullptr, &extensions_count, nullptr);
	if (result != VK_SUCCESS || extensions_count == 0) {
		std::cout << "Could not get the number of device extensions." << std::endl;
		return false;
	}
	availableDeviceExtensions.resize(extensions_count);
	result = vkEnumerateDeviceExtensionProperties(physDevice.get(), nullptr, &extensions_count, availableDeviceExtensions.data());
	if (result != VK_SUCCESS || extensions_count == 0) {
		std::cout << "Could not enumerate device extensions." << std::endl;
		return false;
	}
	return true;
}

bool IsExtensionSupported(const std::vector<VkExtensionProperties>& available_extensions, const char* const extension) {
	for (auto& ext : available_extensions) {
		if (strcmp(ext.extensionName, extension) == 0) {
			return true;
		}
	}
	std::cout << "Extension " << extension << " not supported." << std::endl;
	return false;
}