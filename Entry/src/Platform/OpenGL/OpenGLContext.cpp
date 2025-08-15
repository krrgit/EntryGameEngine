#include "etpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include<glad/glad.h>
#include <GL/GL.h>

namespace Entry {

	Entry::OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ET_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ET_CORE_ASSERT(status, "Failed to initialize Glad!");
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		glfwSwapBuffers(m_WindowHandle);
	}
}