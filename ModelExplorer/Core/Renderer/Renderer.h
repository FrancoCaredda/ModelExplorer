#ifndef _RENDERER_H
#define _RENDERER_H

#include <vulkan/vulkan.h>
#include <vector>
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

	void Init(const VulkanApplicationInfoProvider::ProviderInfo& info,
			  AWindow* window);

	void WaitFor();

	~Renderer();
private:
	void InitVulkan(const VkApplicationInfo& info);
	void InitSurface(GLFWwindow* window);
	void SelectPhysicalDevice();
	void InitDevice();
	void InitSwapchain();
	void InitImageViews();

	VkSurfaceFormatKHR GetSurfaceFormat() const noexcept;
	VkExtent2D GetSurfaceExtent() const noexcept;
	VkPresentModeKHR GetSurfacePresentationMode() const noexcept;

#ifdef _DEBUG
private:
	void InitDebugLayer();
	void LogSupportedInstances() const noexcept;
	void LogSupportedLayers() const noexcept;
	void LogSupportedDeviceExtensions() const noexcept;
	void LogSupportedDeviceLayers() const noexcept;
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
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_Swapchain;
	std::vector<VkImageView> m_ImageViews;
#ifdef _DEBUG
	VkDebugUtilsMessengerEXT m_DebugLayer;
#endif

	GLFWwindow* m_Window;
};

#endif