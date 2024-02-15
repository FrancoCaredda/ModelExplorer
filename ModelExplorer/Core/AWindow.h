#ifndef _AWINDOW_H
#define _AWINDOW_H

#include <GLFW/glfw3.h>
#include <string>

class AWindow
{
public:
	AWindow(const std::string& title, int width, int height, bool fullscreen = false);
	virtual ~AWindow();

	inline const GLFWwindow* GetNative() const noexcept { return m_Handle; }

	inline const std::string& GetTitle() const noexcept { return m_Title; }
	inline int GetWidth() const noexcept { return m_Width; }
	inline int GetHeight() const noexcept { return m_Height; }
protected:
	virtual void Start() {}
	virtual void Update(float deltaTime) = 0;
private:
	std::string m_Title;
	int m_Width;
	int m_Height;

	GLFWwindow* m_Handle;

	friend class AApplication;
};

#endif // !_AWINDOW_H
