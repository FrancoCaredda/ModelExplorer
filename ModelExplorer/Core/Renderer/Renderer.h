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
	Renderer() = default;

	VkInstance m_Instance;
	static Renderer s_Renderer;
};

#endif