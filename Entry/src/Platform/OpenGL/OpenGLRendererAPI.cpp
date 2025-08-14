#include"etpch.h"
#include "OpenGLRendererAPI.h"

#include"Entry/Core/Application.h"
#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

namespace Entry {
	void OpenGLRendererAPI::Init()
	{
		ET_PROFILE_FUNCTION();
	}

	void OpenGLRendererAPI::SetClearColor(const uint32_t color)
	{
		//static_cast<WindowsWindow*>(&Application::Get().GetWindow())->SetClearColor(color);
		float r = ((color & 0xff000000) >> 24) / 255.0f;
		float g = ((color & 0x00ff0000) >> 16) / 255.0f;
		float b = ((color & 0x0000ff00) >> 8) / 255.0f;
		float a = ((color & 0x000000ff) >> 0) / 255.0f;

		glClearColor(r, g, b, a);
	}
	
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount)
	{
		ET_PROFILE_FUNCTION();
	}


}
