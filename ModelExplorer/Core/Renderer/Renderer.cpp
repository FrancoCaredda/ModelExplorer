#include "Renderer.h"

#include <stdexcept>

Renderer Renderer::s_Renderer;

#define BUFFER_SIZE 100

#define ARRAY_SIZE(arr, type) sizeof(arr) / sizeof(type)

#ifdef _DEBUG
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
											 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
											 const VkAllocationCallbacks* pAllocator, 
											 VkDebugUtilsMessengerEXT* pMessenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

	if (func == nullptr)
		return VK_ERROR_UNKNOWN;

	return func(instance, pCreateInfo, pAllocator, pMessenger);
}

static VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pMessenger, const VkAllocationCallbacks* pAllocator)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (func == nullptr)
		return VK_ERROR_UNKNOWN;

	func(instance, pMessenger, pAllocator);
	return VK_SUCCESS;
}
#endif

void Renderer::Init(const VulkanApplicationInfoProvider::ProviderInfo& info)
{
	InitVulkan(info.info);
	InitDebugLayer();
#ifdef _DEBUG
	LogSupportedInstances();
	LogSupportedLayers();
#endif // _DEBUG

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
	instance.enabledExtensionCount = ARRAY_SIZE(extensions, const char*);
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

	instance.enabledLayerCount = ARRAY_SIZE(layers, const char*);
	instance.ppEnabledLayerNames = layers;
#endif // _DEBUG


	if (vkCreateInstance(&instance, nullptr, &m_Instance) != VK_SUCCESS)
		throw std::runtime_error::exception("Vulkan hasn't been initialized!");
}

void Renderer::InitDebugLayer()
{
	VkDebugUtilsMessengerCreateInfoEXT debugMessenger{};
	debugMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugMessenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT	 |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugMessenger.pfnUserCallback = DebugCallback;

	if (CreateDebugUtilsMessengerEXT(m_Instance, &debugMessenger, nullptr, &m_DebugLayer) != VK_SUCCESS)
		throw std::runtime_error::exception("Debug layer hasn't been created!");
}

#ifdef _DEBUG
#include <iostream>

void Renderer::LogSupportedInstances() const noexcept
{
	uint32_t count;
	VkExtensionProperties properties[BUFFER_SIZE] = { 0 };
	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties);

	std::cout << "[Instance Extensions]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].extensionName << "\n";

	std::cout << std::endl;
}

void Renderer::LogSupportedLayers() const noexcept
{
	uint32_t count;
	VkLayerProperties properties[BUFFER_SIZE] = { 0 };
	vkEnumerateInstanceLayerProperties(&count, properties);

	std::cout << "[Instance Layers]: \n";

	for (int i = 0; i < count; i++)
		std::cout << properties[i].layerName << "\n";

	std::cout << std::endl;
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