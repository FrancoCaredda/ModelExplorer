#include "Renderer.h"

#ifdef _DEBUG
#include "DebugUtils.h"
#endif // !_DEBUG

#include <stdexcept>
#include <vector>
#include <algorithm>

Renderer Renderer::s_Renderer;

void Renderer::Init(const VulkanApplicationInfoProvider::ProviderInfo& info,
	AWindow* window)
{
	InitVulkan(info.info);
	InitSurface(const_cast<GLFWwindow*>(window->GetNative()));
#ifdef _DEBUG	
	InitDebugLayer();
	LogSupportedInstances();
	LogSupportedLayers();
#endif // _DEBUG
	SelectPhysicalDevice();
	InitDevice();
	InitSwapchain();

#ifdef _DEBUG	
	LogSupportedDeviceLayers();
	LogSupportedDeviceExtensions();
#endif
}

void Renderer::WaitFor()
{
	vkDeviceWaitIdle(m_Device);
}

Renderer::~Renderer()
{
	vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
#ifdef _DEBUG
	DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugLayer, nullptr);
#endif // !_DEBUG

	vkDestroyInstance(m_Instance, nullptr);
}

void Renderer::InitVulkan(const VkApplicationInfo& info)
{
	const char* extensions[] = {
		"VK_EXT_debug_utils",
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	const char* layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	VkInstanceCreateInfo instance{};
	instance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance.enabledExtensionCount = sizeof(extensions) / sizeof(const char*);
	instance.ppEnabledExtensionNames = extensions;
	instance.enabledLayerCount = 0;
	instance.ppEnabledLayerNames = nullptr;
	instance.pApplicationInfo = &info;

#ifdef _DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugMessenger{};
	debugMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugMessenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugMessenger.pfnUserCallback = DebugCallback;

	instance.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessenger;

	instance.enabledLayerCount = sizeof(layers) / sizeof(const char*);
	instance.ppEnabledLayerNames = layers;
#endif // _DEBUG

	if (vkCreateInstance(&instance, nullptr, &m_Instance) != VK_SUCCESS)
		throw std::runtime_error::exception("Vulkan hasn't been initialized!");
}

void Renderer::InitSurface(GLFWwindow* window)
{
	m_Window = window;
	if (glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface) != VK_SUCCESS)
		throw std::runtime_error::exception("Surface hasn't been created!");
}

void Renderer::SelectPhysicalDevice()
{
	std::vector<VkPhysicalDevice> devices;
	uint32_t deviceCount;

	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	devices.resize(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	for (auto device : devices)
	{
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};

		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		if ((properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
			(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU))
		{
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

			for (int i = 0; i < queueFamilyProperties.size(); i++)
			{
				VkBool32 supportsPresentation = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &supportsPresentation);

				if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					m_Indices.graphicsQueueIndex = i;

				if (supportsPresentation)
					m_Indices.presentationQueueIndex = i;
			}

			if (m_Indices.presentationQueueIndex.has_value())
			{			
				m_PhysicalDevice = device;
				break;
			}

			m_Indices.graphicsQueueIndex.reset();
			m_Indices.presentationQueueIndex.reset();
		}
	}

	if (m_PhysicalDevice == nullptr)
		throw std::runtime_error::exception("None of the physical devices meets requirements");
}

void Renderer::InitDevice()
{
	float priority = 1.0f;
	uint32_t queueFamilies[] = { m_Indices.graphicsQueueIndex.value(), m_Indices.presentationQueueIndex.value() };
	uint32_t queueFamilyCount = sizeof(queueFamilies) / sizeof(uint32_t);

	std::vector<VkDeviceQueueCreateInfo> m_QueueInfos;
	m_QueueInfos.reserve(queueFamilyCount);

	for (int i = 0; i < queueFamilyCount; i++)
	{
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueFamilies[i];
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &priority;

		m_QueueInfos.push_back(queueInfo);
	}

	const char* extensions[] = {
		"VK_KHR_swapchain"
	};

	VkDeviceCreateInfo deviceInfo{};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.ppEnabledExtensionNames = extensions;
	deviceInfo.enabledExtensionCount = sizeof(extensions) / sizeof(const char*);
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.queueCreateInfoCount = m_QueueInfos.size();
	deviceInfo.pQueueCreateInfos = m_QueueInfos.data();

	if (vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device) != VK_SUCCESS)
		throw std::runtime_error::exception("Device hasn't been initialized!");

}

void Renderer::InitSwapchain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);

	VkExtent2D extent = GetSurfaceExtent();
	VkSurfaceFormatKHR format = GetSurfaceFormat();
	VkPresentModeKHR presentMode = GetSurfacePresentationMode();

	VkSwapchainCreateInfoKHR swapchain{};
	swapchain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain.surface = m_Surface;
	swapchain.clipped = VK_TRUE;
	swapchain.minImageCount = surfaceCapabilities.minImageCount;
	swapchain.imageFormat = format.format;
	swapchain.imageColorSpace = format.colorSpace;
	swapchain.imageExtent = extent;
	swapchain.presentMode = presentMode;
	swapchain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain.imageArrayLayers = 1;
	swapchain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain.oldSwapchain = nullptr;

	uint32_t queueFamilyIndices[] = { m_Indices.graphicsQueueIndex.value(), m_Indices.presentationQueueIndex.value() };
	if (queueFamilyIndices[0] == queueFamilyIndices[1])
	{
		swapchain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain.pQueueFamilyIndices = queueFamilyIndices;
		swapchain.queueFamilyIndexCount = 1;
	}
	else
	{
		swapchain.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain.pQueueFamilyIndices = queueFamilyIndices;
		swapchain.queueFamilyIndexCount = 2;
	}

	if (vkCreateSwapchainKHR(m_Device, &swapchain, nullptr, &m_Swapchain) != VK_SUCCESS)
		throw std::runtime_error::exception("Swapchain hasn't been created!");
}

VkSurfaceFormatKHR Renderer::GetSurfaceFormat() const noexcept
{
	uint32_t count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, formats.data());

	for (auto& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8_SRGB &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	return formats[0];
}

VkExtent2D Renderer::GetSurfaceExtent() const noexcept
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);

	if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}

	int width;
	int height;
	glfwGetFramebufferSize(m_Window, &width, &height);

	VkExtent2D extent{};
	extent.width = std::clamp(static_cast<uint32_t>(width), surfaceCapabilities.minImageExtent.width, 
															surfaceCapabilities.currentExtent.width);
	extent.height = std::clamp(static_cast<uint32_t>(height), surfaceCapabilities.minImageExtent.height,
															  surfaceCapabilities.currentExtent.height);

	return extent;
}

VkPresentModeKHR Renderer::GetSurfacePresentationMode() const noexcept
{
	uint32_t count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, presentModes.data());

	if (std::find(presentModes.cbegin(),
		presentModes.cend(),
		VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.cend())
		return VK_PRESENT_MODE_MAILBOX_KHR;

	return VK_PRESENT_MODE_FIFO_KHR;
}

#ifdef _DEBUG
#include <iostream>

void Renderer::LogSupportedInstances() const noexcept
{
	uint32_t count;
	std::vector<VkExtensionProperties> properties;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	properties.resize(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());


	std::cout << "[Instance Extensions]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].extensionName << "\n";

	std::cout << std::endl;
}

void Renderer::LogSupportedLayers() const noexcept
{
	uint32_t count;
	std::vector<VkLayerProperties> properties;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	properties.resize(count);
	vkEnumerateInstanceLayerProperties(&count, properties.data());

	std::cout << "[Instance Layers]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].layerName << "\n";

	std::cout << std::endl;
}

void Renderer::LogSupportedDeviceExtensions() const noexcept
{
	uint32_t count;
	std::vector<VkExtensionProperties> properties;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr);

	properties.resize(count);
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, properties.data());

	std::cout << "[Device Extensions]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].extensionName << "\n";

	std::cout << std::endl;
}

void Renderer::LogSupportedDeviceLayers() const noexcept
{
	uint32_t count;
	std::vector<VkLayerProperties> properties;
	vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &count, nullptr);

	properties.resize(count);
	vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &count, properties.data());

	std::cout << "[Device Layers]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].layerName << "\n";

	std::cout << std::endl;
}

void Renderer::InitDebugLayer()
{
	VkDebugUtilsMessengerCreateInfoEXT debugMessenger{};
	debugMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugMessenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugMessenger.pfnUserCallback = DebugCallback;

	if (CreateDebugUtilsMessengerEXT(m_Instance, &debugMessenger, nullptr, &m_DebugLayer) != VK_SUCCESS)
		throw std::runtime_error::exception("Debug layer hasn't been created!");
}

VkBool32 VKAPI_PTR Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
										   VkDebugUtilsMessageTypeFlagsEXT messageTypes, 
										   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
										   void* pUserData)
{
	std::cout << pCallbackData->pMessage << "\n";

	return true;
}
#endif // !