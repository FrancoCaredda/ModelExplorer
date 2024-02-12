#ifndef _AAPLICATION_H
#define _AAPLICATION_H

#include "AWindow.h"

class AApplication
{
public:
	AApplication();
	~AApplication();

	void SetCurrentWindow(AWindow* window) noexcept;
	inline AWindow* GetCurrentWindow() const noexcept { return m_Window; }

	void Init();
	void Run();
private:
	void InitGLFW();
private:
	AWindow* m_Window = nullptr;
};

#endif // _AAPPLICATION_H