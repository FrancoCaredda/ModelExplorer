#include "Renderer.h"

#include <stdexcept>

Renderer Renderer::s_Renderer;

void Renderer::Init(const VulkanApplicationInfoProvider::ProviderInfo& info)
{
	VkInstanceCreateInfo instance{};
	instance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance.enabledExtensionCount = 0;
	instance.ppEnabledExtensionNames = nullptr;
	instance.enabledLayerCount = 0;
	instance.ppEnabledLayerNames = nullptr;
	instance.pApplicationInfo = &info.info;

	if (vkCreateInstance(&instance, nullptr, &m_Instance) != VK_SUCCESS)
		throw std::runtime_error::exception("Vulkan hasn't been initialized!");
}

Renderer::~Renderer()
{
	vkDestroyInstance(m_Instance, nullptr);
}
