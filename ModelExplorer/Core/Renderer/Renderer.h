#ifndef _RENDERER_H
#define _RENDERER_H

#include <vulkan/vulkan.h>
#include <optional>

#include "Core/Provider/VulkanApplicationInfoProvider.h"

typedef struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsQueueIndex;
	std::optional<uint32_t> presentationQueueIndex;
} QueueFamilyIndices_t;

class Renderer
{
public:
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;

	inline static Renderer& GetInstance() noexcept { return s_Renderer; }

	void Init(const VulkanApplicationInfoProvider::ProviderInfo& info);

	~Renderer();
private:
	void InitVulkan(const VkApplicationInfo& info);
	void SelectPhysicalDevice();
	void InitDevice();

#ifdef _DEBUG
private:
	void InitDebugLayer();
	void LogSupportedInstances() const noexcept;
	void LogSupportedLayers() const noexcept;
	static VkBool32 VKAPI_PTR DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
		void*											 pUserData
	);
#endif // !_DEBUG
private:
	Renderer() = default;
	static Renderer s_Renderer;

	QueueFamilyIndices_t m_Indices;

	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
#ifdef _DEBUG
	VkDebugUtilsMessengerEXT m_DebugLayer;
#endif
};

#endif