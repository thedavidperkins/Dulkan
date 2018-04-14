#ifndef DK_WINDOW_H
#define DK_WINDOW_H

#include "DkCommon.h"

class DkPhysicalDevice;
class DkQueue;
class DkSwapchain;
class DkSemaphore;

struct DkPresentInfo {
	DkSwapchain* swapchain;
	uint index;
};

struct WindowParameters {
	HINSTANCE HInstance;
	HWND HWnd;
};

namespace {
	enum UserMessage {
		USER_MESSAGE_RESIZE = WM_USER + 1,
		USER_MESSAGE_QUIT,
		USER_MESSAGE_MOUSE_CLICK,
		USER_MESSAGE_MOUSE_MOVE,
		USER_MESSAGE_MOUSE_WHEEL
	};
}

class DkWindow {
public:
	DkWindow(DkPhysicalDevice& physDevice);
	~DkWindow() { finalize(); }

	bool init();
	void finalize();
	bool resize();

	// set init values
	void setWindowRect(VkRect2D windowRect);

	// Setter
	void setSurfaceExtent(VkExtent2D surfaceExtent) { m_surfaceExtent = surfaceExtent; }

	// Getters
	VkSurfaceKHR getSurface() { return m_presentSurface; }
	VkExtent2D getExtent() { return m_surfaceExtent; }
	VkSurfaceCapabilitiesKHR getSurfaceCapabilities() { return m_surfaceCapabilities; }
	HWND getHWnd() { return m_windowParams.HWnd; }
	
	bool getNearestAvailableSurfaceFormat(VkSurfaceFormatKHR& formatInOut);
	bool checkPresentMode(VkPresentModeKHR& modeInOut);

	bool present(
		DkQueue& queue,

		const std::vector<DkSemaphore*>& semaphores,
		const std::vector<DkPresentInfo>& presentInfo
	);

	DkWindow(const DkWindow& rhs) = delete;
	DkWindow& operator=(const DkWindow& rhs) = delete;
private:
	// helper functions
	bool _refreshSurfaceCapabilities();

	// Set on construction
	DkPhysicalDevice& m_physDevice;

	// Set before init
	VkRect2D m_windowRect;

	// Set by init
	WindowParameters m_windowParams;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	VkSurfaceKHR m_presentSurface;
	bool m_initialized;

	// Track surface size
	VkExtent2D m_surfaceExtent;
};

#endif//DK_WINDOW_H

