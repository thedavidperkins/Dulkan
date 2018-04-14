#include "DkWindow.h"
#include "DkApplication.h"
#include "DkPhysicalDevice.h"
#include "DkSemaphore.h"
#include "DkSwapchain.h"

// WindowProcedure callback lifted from Vulkan Cookbook sample code

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_LBUTTONDOWN:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 1);
		break;
	case WM_LBUTTONUP:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 0);
		break;
	case WM_RBUTTONDOWN:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 1);
		break;
	case WM_RBUTTONUP:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 0);
		break;
	case WM_MOUSEMOVE:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		PostMessage(hWnd, USER_MESSAGE_MOUSE_WHEEL, HIWORD(wParam), 0);
		break;
	case WM_SIZE:
	case WM_EXITSIZEMOVE:
		PostMessage(hWnd, USER_MESSAGE_RESIZE, wParam, lParam);
		break;
	case WM_KEYDOWN:
		if (VK_ESCAPE == wParam) {
			PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
		}
		break;
	case WM_CLOSE:
		PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DkWindow::DkWindow(DkPhysicalDevice& physDevice) :
	m_physDevice(physDevice),
	m_windowRect({ {50, 25}, {1280, 800} }),
	m_windowParams({ NULL, NULL }),
	m_surfaceCapabilities(),
	m_presentSurface(VK_NULL_HANDLE),
	m_initialized(false),
	m_surfaceExtent(m_windowRect.extent)
{}

void DkWindow::setWindowRect(VkRect2D windowRect) {
	m_windowRect = windowRect;
}

bool DkWindow::init() {
	if (m_initialized) {
		finalize();
	}

	m_windowParams.HInstance = GetModuleHandle(nullptr);
	char className[] = "DkWindowClass";
	WNDCLASSEX windowClass = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		windowProcedure,
		0,
		0,
		m_windowParams.HInstance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		className,
		nullptr
	};

	if (!RegisterClassEx(&windowClass)) {
		std::cout << "Failed to register window class." << std::endl;
		return false;
	}

	m_windowParams.HWnd = CreateWindow(className, DkApplication::getInstance().getAppName().c_str(),
		WS_OVERLAPPEDWINDOW, m_windowRect.offset.x, m_windowRect.offset.y, m_windowRect.extent.width, 
		m_windowRect.extent.height, nullptr, nullptr, m_windowParams.HInstance, nullptr);

	if (!m_windowParams.HWnd) {
		std::cout << "Failed to get window handle." << std::endl;
		return false;
	}

	VkWin32SurfaceCreateInfoKHR surfInfo = {
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		m_windowParams.HInstance,
		m_windowParams.HWnd
	};

	if (vkCreateWin32SurfaceKHR(getVkInstance(), &surfInfo, nullptr, &m_presentSurface) != VK_SUCCESS
		|| m_presentSurface == VK_NULL_HANDLE) {
		std::cout << "Failed to create present surface." << std::endl;
		return false;
	}
	if (!_refreshSurfaceCapabilities()) return false;
	m_initialized = true;
	return true;
}

bool DkWindow::_refreshSurfaceCapabilities() {
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physDevice.get(), m_presentSurface, &m_surfaceCapabilities) != VK_SUCCESS) {
		std::cout << "Failed to get surface capabilities." << std::endl;
		return false;
	}
	return true;
}

void DkWindow::finalize() {
	if (m_presentSurface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(getVkInstance(), m_presentSurface, nullptr);
		m_presentSurface = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

bool DkWindow::getNearestAvailableSurfaceFormat(VkSurfaceFormatKHR& formatInOut) {
	uint nFormats;
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physDevice.get(), m_presentSurface, &nFormats, nullptr) != VK_SUCCESS) {
		std::cout << "Failed to get available surface format count." << std::endl;
		return false;
	}
	std::vector<VkSurfaceFormatKHR> formats;
	formats.resize(nFormats);
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physDevice.get(), m_presentSurface, &nFormats, formats.data()) != VK_SUCCESS) {
		std::cout << "Failed to get available surface formats." << std::endl;
		return false;
	}

	if (nFormats == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		return true;
	}

	for (auto& checkFormat : formats) {
		if (formatInOut.colorSpace == checkFormat.colorSpace && formatInOut.format == checkFormat.format) {
			return true;
		}
	}
	std::cout << "Failed to find exact surface format desired. Seeking another surface format with the same format member." << std::endl;
	for (auto& checkFormat : formats) {
		if (formatInOut.format == checkFormat.format) {
			formatInOut = checkFormat;
			return true;
		}
	}
	std::cout << "Failed to find surface format with the desired format member. Seeking any available surface format." << std::endl;
	formatInOut = formats[0];
	return true;
}

bool DkWindow::checkPresentMode(VkPresentModeKHR& modeInOut) {
	uint nPresModes;
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physDevice.get(), m_presentSurface, &nPresModes, nullptr) != VK_SUCCESS) {
		std::cout << "Failed to get physical device present modes count. Defaulting to FIFO." << std::endl;
		modeInOut = VK_PRESENT_MODE_FIFO_KHR;
		return false;
	}
	else {
		std::vector<VkPresentModeKHR> modes;
		modes.resize(nPresModes);
		if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physDevice.get(), m_presentSurface, &nPresModes, modes.data()) != VK_SUCCESS) {
			std::cout << "Failed to get physical device present modes. Defaulting to FIFO." << std::endl;
			modeInOut = VK_PRESENT_MODE_FIFO_KHR;
			return false;
		}
		else {
			if (std::find(modes.begin(), modes.end(), modeInOut) == modes.end()) {
				std::cout << "Couldn't use the requested presentation mode. Defaulting to FIFO." << std::endl;
				modeInOut = VK_PRESENT_MODE_FIFO_KHR;
				return false;
			}
		}
	}
	return true;
}

bool DkWindow::present(
	DkQueue& queue,
	const std::vector<DkSemaphore*>& semaphores,
	const std::vector<DkPresentInfo>& presentInfo
) {
	std::vector<VkSemaphore> sems;
	for (auto& sem : semaphores) {
		sems.push_back(sem->get());
	}

	std::vector<VkSwapchainKHR> swapchains;
	std::vector<uint> imgIndices;
	for (auto& inf : presentInfo) {
		swapchains.push_back(inf.swapchain->get());
		imgIndices.push_back(inf.index);
	}

	VkPresentInfoKHR info = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		(uint)sems.size(),
		sems.data(),
		(uint)swapchains.size(),
		swapchains.data(),
		imgIndices.data()
	};

	if (vkQueuePresentKHR(queue.get(), &info) != VK_SUCCESS) {
		std::cout << "Failed to present image." << std::endl;
		return false;
	}
	return true;
}

bool DkWindow::resize() {
	return _refreshSurfaceCapabilities();
}