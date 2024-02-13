#ifndef _RENDERER_H
#define _RENDERER_H

#include <vulkan/vulkan.h>

#include "Core/Provider/VulkanApplicationInfoProvider.h"

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
	void InitDebugLayer();
#ifdef _DEBUG
private:
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


	VkInstance m_Instance;
#ifdef _DEBUG
	VkDebugUtilsMessengerEXT m_DebugLayer;
#endif
};

#endif