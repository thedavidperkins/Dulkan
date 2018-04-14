#include "DkInstance.h"
#include "DkUtils.h"

DkInstance::DkInstance() :
	m_appName("MyVulkanApplication"),
	m_desiredInstExts({ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME }),
	m_initialized(false),
	m_instance(VK_NULL_HANDLE)
{}

void DkInstance::setAppName(std::string appName) {
	if (m_initialized) {
		std::cout << "Cannot alter application name after initialization." << std::endl;
		return;
	}
	m_appName = appName;
}

void DkInstance::setDesiredExtensions(const std::vector<const char*> desiredExts) {
	if (m_initialized) {
		std::cout << "Cannot alter instance extensions name after initialization." << std::endl;
		return;
	}
	m_desiredInstExts.clear();
	for (auto& ext : desiredExts) {
		m_desiredInstExts.push_back(ext);
	}
}

bool DkInstance::init() {
	if (m_initialized) {
		finalize();
	}

	// Check for extension support
	std::vector<VkExtensionProperties> available;
	if (!getAvailableInstanceExtensions(available)) return false;
	for (auto& ext : m_desiredInstExts) {
		if (!IsExtensionSupported(available, ext)) return false;
	}

	// Create instance
	VkApplicationInfo appInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		m_appName.c_str(),
		VK_MAKE_VERSION(1, 0, 0),
		"DkBase",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0)
	};

	VkInstanceCreateInfo instInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
		0,
		nullptr,
		(uint)m_desiredInstExts.size(),
		m_desiredInstExts.data()
	};

	VkResult result = vkCreateInstance(&instInfo, nullptr, &m_instance);
	if (result != VK_SUCCESS) {
		std::cout << "Failed to create instance." << std::endl;
		return false;
	}

	// Load instance level functions
	if (!loadInstanceFns(m_instance, m_desiredInstExts)) return false;

	m_initialized = true;
	return true;
}

void DkInstance::finalize() {
	if (m_instance != VK_NULL_HANDLE) {
		vkDestroyInstance(m_instance, nullptr);
		m_instance = VK_NULL_HANDLE;
	}
	m_initialized = false;
}