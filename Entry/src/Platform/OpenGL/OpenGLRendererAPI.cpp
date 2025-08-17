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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* window)
	{
		// Make the window's context current before touching GL state
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window);
		glfwMakeContextCurrent(glfwWin);

		glViewport(x, y, width, height);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount)
	{
		//ET_PROFILE_FUNCTION();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
	}


}
