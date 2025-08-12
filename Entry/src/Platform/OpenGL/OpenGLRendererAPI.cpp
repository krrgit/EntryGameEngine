#include"etpch.h"
#include "OpenGLRendererAPI.h"

#include"Entry/Core/Application.h"
#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>

namespace Entry {
	void OpenGLRendererAPI::Init()
	{
		ET_PROFILE_FUNCTION();
	}

	void OpenGLRendererAPI::SetClearColor(const uint32_t color)
	{
		static_cast<WindowsWindow*>(&Application::Get().GetWindow())->SetClearColor(color);
	}
	
	void OpenGLRendererAPI::Clear()
	{
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount)
	{
		ET_PROFILE_FUNCTION();
	}


}
