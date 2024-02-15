#include "AApplication.h"

#include "Provider/VulkanApplicationInfoProvider.h"
#include "Renderer/Renderer.h"

#include <stdexcept>

static AApplication* s_Application = nullptr;

AApplication::AApplication(const std::string& name, uint32_t version) 
	: m_Name(name), m_Version(version) 
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

	Renderer::GetInstance().WaitFor();
}

void AApplication::InitGLFW()
{
	if (!glfwInit())
		throw std::runtime_error::exception("GLFW hasn't been initialized!");
}
