#pragma once

#include "Platform/Windows/GraphicsContext.h"

struct GLFWwindow;

namespace Entry {
	class OpenGLContext : public GraphicsContext 
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;

	};
}