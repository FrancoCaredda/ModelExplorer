#ifndef _IPROVIDER_H
#define _IPROVIDER_H

template<typename T, typename V>
class IProvider
{
public:
	struct ProviderInfo
	{
		V info;
	};

	virtual ProviderInfo Provide(const T& from) const noexcept = 0;
};

#endif // !_IPROVIDER_H
