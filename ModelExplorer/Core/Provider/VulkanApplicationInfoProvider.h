#ifndef _VULKAN_APPLICATION_INFO_PROVIDER_H
#define _VULKAN_APPLICATION_INFO_PROVIDER_H

#include "IProvider.h"
#include "Core/AApplication.h"

#include <vulkan/vulkan.h>

class VulkanApplicationInfoProvider : public IProvider<AApplication, VkApplicationInfo>
{
public:
	virtual ProviderInfo Provide(const AApplication& from) const noexcept override;
};

#endif // !_VULKAN_APPLICATION_INFO_PROVIDER_H
