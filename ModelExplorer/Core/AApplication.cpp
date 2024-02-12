#include "AApplication.h"

#include <stdexcept>

static AApplication* s_Application = nullptr;

AApplication::AApplication()
{
	if (s_Application != nullptr)
		throw std::runtime_error::exception("Only one application can be created within a project!");

	s_Application = this;
}

AApplication::~AApplication()
{
	glfwTerminate();
}

void AApplication::SetCurrentWindow(AWindow* window) noexcept
{
	m_Window = window;
}

void AApplication::Init()
{
	InitGLFW();
}

void AApplication::Run()
{
	m_Window->Start();

	while (!glfwWindowShouldClose(m_Window->m_Handle))
	{
		glfwPollEvents();
		m_Window->Update(0);
	}
}

void AApplication::InitGLFW()
{
	if (!glfwInit())
		throw std::runtime_error::exception("GLFW hasn't been initialized!");
}
