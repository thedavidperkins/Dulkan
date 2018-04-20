#include "DkApplication.h"

DkApplication::DkApplication() :
	m_vkLibrary(nullptr),
	m_instance(),
	m_physDevice(m_instance),
	m_window(m_instance, m_physDevice),
	m_device(m_physDevice),
	m_queues(),
	m_commandPools()
{}

bool DkApplication::vulkanInit() {
	// Load global functions
	if (!cnctLoaderLib(m_vkLibrary)) return false;
	if (!loadExportedFn(m_vkLibrary)) return false;
	if (!loadGlobalFns()) return false;

	// Init VkInstance
	if (!m_instance.init()) return false;

	// Init physical device
	if (!m_physDevice.init()) return false;

	// Init presentation surface and window
	if (!m_window.init()) return false;

	// Get desired queue indices
	std::vector<uint> indices;
	if (!_findQueueIndices(indices)) return false;
	m_device.setQueueIndices(indices);

	// Init logical device
	if (!m_device.init()) return false;

	// Get queues from device
	_getDeviceQueues();

	// Init command pools: one for each queue
	m_commandPools.clear();
	for (auto& queue : m_queues) {
		DkCommandPool* newPool = new DkCommandPool(m_device, queue);
		if (!newPool->init()) return false;
		m_commandPools.push_back(newPool);
	}

	return true;
}

void DkApplication::render() {
	ShowWindow(m_window.getHWnd(), SW_SHOWNORMAL);
	UpdateWindow(m_window.getHWnd());

	MSG message;
	bool loop = true;

	while (loop) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			switch (message.message) {
			case USER_MESSAGE_MOUSE_CLICK:
			case USER_MESSAGE_MOUSE_MOVE:
			case USER_MESSAGE_MOUSE_WHEEL:
			case USER_MESSAGE_RESIZE:
				loop = resize();
				break;
			case USER_MESSAGE_QUIT:
				loop = false;
				break;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			if (!draw()) loop = false;
		}
	}
}

// Assume one queue from each family
bool DkApplication::_findQueueIndices(std::vector<uint>& indices) {
	indices.clear();

	int test = m_physDevice.findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
	if (test < 0) return false;
	m_queues[DK_GRAPHICS_QUEUE].setFamIndex((uint)test);
	indices.push_back((uint)test);

	test = m_physDevice.findQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
	if (test < 0) return false;
	m_queues[DK_COMPUTE_QUEUE].setFamIndex((uint)test);
	if (indices[0] != (uint)test) indices.push_back((uint)test);

	test = m_physDevice.findQueueFamilyPresentIndex(m_window);
	if (test < 0) return false;
	m_queues[DK_PRESENT_QUEUE].setFamIndex((uint)test);
	if (indices[0] != (uint)test && indices[1] != (uint)test) indices.push_back((uint)test);

	for (uint iter = 0; iter < (uint)DK_NUM_QUEUE_TYPES; ++iter) {
		m_queues[iter].setType((DkQueueType)iter);
	}
	return true;
}

void DkApplication::_getDeviceQueues() {
	for (uint iter = 0; iter < (uint)DK_NUM_QUEUE_TYPES; ++iter) {
		vkGetDeviceQueue(m_device.get(), m_queues[iter].getFamilyIndex(), 0, &(m_queues[iter].get()));
	}
}

void DkApplication::vulkanFinalize() {
	for (auto& pool : m_commandPools) {
		pool->finalize();
		delete pool;
		pool = nullptr;
	}
	m_commandPools.clear();
	m_device.finalize();
	m_physDevice.finalize();
	m_window.finalize();
	m_instance.finalize();
	FreeLibrary(m_vkLibrary);
}
