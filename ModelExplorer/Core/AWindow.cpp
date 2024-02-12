#include "AWindow.h"

#include <stdexcept>

AWindow::AWindow(const std::string& title, int width, int height, bool fullscreen)
	: m_Title(title), m_Width(width), m_Height(height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_Handle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), (fullscreen) ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	if (!m_Handle)
		throw std::runtime_error::exception("Window hasn't been created!");
}

AWindow::~AWindow()
{
	glfwDestroyWindow(m_Handle);
}
