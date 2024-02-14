#ifndef _DEBUG_UTILS_H
#define _DEBUG_UTILS_H

#include <vulkan/vulkan.h>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger);

VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, 
	VkDebugUtilsMessengerEXT pMessenger, 
	const VkAllocationCallbacks* pAllocator);

#endif // !_DEBUG_UTILS_H
