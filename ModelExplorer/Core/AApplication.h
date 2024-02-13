#ifndef _AAPLICATION_H
#define _AAPLICATION_H

#include "AWindow.h"

#include <string>

class AApplication
{
public:
	AApplication(const std::string& name, uint32_t version);
	~AApplication();

	void SetCurrentWindow(AWindow* window) noexcept;

	inline AWindow* GetCurrentWindow() const noexcept { return m_Window; }
	inline const std::string& GetName() const noexcept { return m_Name; }
	inline uint32_t GetVersion() const noexcept { return m_Version; }

	inline static uint32_t MakeVersion(uint32_t variant, uint32_t major, uint32_t minor, uint32_t patch)
	{ 
		return ((((uint32_t)(variant)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)));
	}

	void Init();
	void Run();
private:
	void InitGLFW();
	void InitRenderer();
private:
	AWindow* m_Window = nullptr;
	std::string m_Name;
	uint32_t m_Version;
};

#endif // _AAPPLICATION_H