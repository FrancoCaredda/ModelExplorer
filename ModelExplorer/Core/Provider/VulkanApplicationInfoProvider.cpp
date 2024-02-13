#include "VulkanApplicationInfoProvider.h"

VulkanApplicationInfoProvider::ProviderInfo VulkanApplicationInfoProvider::Provide(const AApplication& from) const noexcept
{
	VulkanApplicationInfoProvider::ProviderInfo providerInfo{};
	providerInfo.info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	providerInfo.info.pApplicationName = from.GetName().c_str();
	providerInfo.info.applicationVersion = from.GetVersion();
	providerInfo.info.pEngineName = nullptr;
	providerInfo.info.apiVersion = VK_API_VERSION_1_0;
	providerInfo.info.engineVersion = 0;

	return providerInfo;
}
