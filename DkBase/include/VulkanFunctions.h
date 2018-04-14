#ifndef VULKAN_FUNCTIONS_H
#define VULKAN_FUNCTIONS_H
#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#define LIBRARY_TYPE HMODULE

#include <vector>
#include <vulkan.h>

#define EXPORTED_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;

#include "inline/ListOfVulkanFunctions.inl"

// Function loading from DLL
bool cnctLoaderLib(LIBRARY_TYPE& vkLibrary);
bool loadExportedFn(LIBRARY_TYPE& vkLibrary);
bool loadGlobalFns();
bool loadInstanceFns(VkInstance instance, const std::vector<const char*>& enabled_extensions);
bool loadDeviceFns(VkDevice device, const std::vector<const char*>& enabled_extensions);

#endif//VULKAN_FUNCTIONS_H
