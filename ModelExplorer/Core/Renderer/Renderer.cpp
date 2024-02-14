#include "Renderer.h"

#ifdef _DEBUG
#include "DebugUtils.h"
#endif // !_DEBUG

#include <stdexcept>
#include <vector>

Renderer Renderer::s_Renderer;

void Renderer::Init(const VulkanApplicationInfoProvider::ProviderInfo& info)
{
	InitVulkan(info.info);
#ifdef _DEBUG	
	InitDebugLayer();
	LogSupportedInstances();
	LogSupportedLayers();
#endif // _DEBUG
	SelectPhysicalDevice();
	InitDevice();
}

Renderer::~Renderer()
{
#ifdef _DEBUG
	DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugLayer, nullptr);
#endif // !_DEBUG

	vkDestroyInstance(m_Instance, nullptr);
}

void Renderer::InitVulkan(const VkApplicationInfo& info)
{
	const char* extensions[] = {
		"VK_EXT_debug_utils"
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

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

			for (int i = 0; i < queueFamilyProperties.size(); i++)
			{
				if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					m_Indices.graphicsQueueIndex = i;
			}

			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == nullptr)
		throw std::runtime_error::exception("None of the physical devices meets requirements");
}

void Renderer::InitDevice()
{
	float priority = 1.0f;
	uint32_t queueFamilies[] = { m_Indices.graphicsQueueIndex.value() };
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

	VkDeviceCreateInfo deviceInfo{};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.queueCreateInfoCount = m_QueueInfos.size();
	deviceInfo.pQueueCreateInfos = m_QueueInfos.data();

	if (vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device) != VK_SUCCESS)
		throw std::runtime_error::exception("Device hasn't been initialized!");

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