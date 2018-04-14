#include <iostream>
#include "VulkanFunctions.h"

#define EXPORTED_VULKAN_FUNCTION( name ) PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;

#include "inline/ListOfVulkanFunctions.inl"

//====================================================================
//
//					Function loading from DLL
//
//====================================================================

bool cnctLoaderLib(LIBRARY_TYPE& vkLibrary) {
	vkLibrary = LoadLibrary("vulkan-1.dll");
	if (vkLibrary == nullptr) {
		std::cout << "Couldn't load Vulkan DLL." << std::endl;
		return false;
	}
	return true;
}

bool loadExportedFn(LIBRARY_TYPE& vkLibrary) {

#define EXPORTED_VULKAN_FUNCTION( name )							\
name = (PFN_##name)GetProcAddress( vkLibrary, #name );			\
if(name == nullptr) {												\
	std::cout << "Could not load exported Vulkan function named: "	\
	#name << std::endl;												\
	return false;													\
}

#include "inline/ListOfVulkanFunctions.inl"
	return true;
}

bool loadGlobalFns() {
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )					\
name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);		\
if(name == nullptr) {											\
	std::cout << "Could not load global-level function named: "	\
	#name << std::endl;											\
	return false;												\
}

#include "inline/ListOfVulkanFunctions.inl"

	return true;
}

bool loadInstanceFns(VkInstance instance, const std::vector<const char*>& enabled_extensions) {
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)						\
name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);			\
if(name == nullptr) {												\
	std::cout << "Could not load instance-level function named: "	\
	#name << std::endl;												\
	return false;													\
}

#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)		\
for(auto& enabled_extension : enabled_extensions) {							\
	if(std::string(enabled_extension) == std::string(extension)) {			\
		name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);			\
		if(name == nullptr) {												\
			std::cout << "Could not load instance-level function named: "	\
			#name << " from extension " << enabled_extension << std::endl;	\
			return false;													\
		}																	\
	}																		\
}

#include "inline/ListOfVulkanFunctions.inl"

	return true;
}

bool loadDeviceFns(VkDevice device, const std::vector<const char*>& enabled_extensions) {
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)						\
name = (PFN_##name)vkGetDeviceProcAddr(device, #name);			\
if(name == nullptr) {											\
	std::cout << "Could not load device-level function named: "	\
	#name << std::endl;											\
	return false;												\
}

#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)		\
for(auto& enabled_extension : enabled_extensions) {							\
	if(std::string(enabled_extension) == std::string(extension)) {			\
		name = (PFN_##name)vkGetDeviceProcAddr(device, #name);				\
		if(name == nullptr) {												\
			std::cout << "Could not load device-level function named: "		\
			#name << " from extension " << enabled_extension << std::endl;	\
			return false;													\
		}																	\
	}																		\
}

#include "inline/ListOfVulkanFunctions.inl"

	return true;
}