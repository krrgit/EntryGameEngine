#include "etpch.h"
#include "WindowsWindow.h"

namespace Entry
{
	static uint8_t s_GLFWWindowCount = 0;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		ET_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		ET_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			ET_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			ET_CORE_ASSERT(success, "Could not initialize GLFW!");
			//glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
	void WindowsWindow::OnUpdate(Timestep ts)
	{
		FrameDrawOn();

		LayerStackOnUpdate(ts);
	}

	void WindowsWindow::FrameDrawOn()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
		glfwMakeContextCurrent(m_Window);

		glClear(GL_COLOR_BUFFER_BIT);
	}

	void WindowsWindow::LayerStackOnUpdate(Timestep ts, uint16_t screenSide)
	{
		{
			ET_PROFILE_SCOPE("LayerStack OnUpdate");
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(ts, screenSide);
		}
		{
			ET_PROFILE_SCOPE("LayerStack OnImGuiRender");
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
		}
	}

	void WindowsWindow::ScanHIDEvents()
	{
	}
	void WindowsWindow::FrameEnd()
	{

	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		ET_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
	void WindowsWindow::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}
	void WindowsWindow::PushOverlay(Layer* layer)
	{
		// Add Layer to prepare 2D overlays
		if (!m_Data.has2D)
		{
			//m_LayerStack.PushOverlay(new C2DPrepareLayer());
			m_Data.has2D = true;
		}

		m_LayerStack.PushOverlay(layer);

	}

	void WindowsWindow::OnEvent(Event& e)
	{
	}

	void WindowsWindow::SetClearColor(uint32_t color)
	{
		float r = ((color & 0xff000000) >> 24) / 255.0f;
		float g = ((color & 0x00ff0000) >> 16) / 255.0f;
		float b = ((color & 0x0000ff00) >> 8) / 255.0f;
		float a = ((color & 0x000000ff) >> 0) / 255.0f;
		
		m_ClearColor.r = r;
		m_ClearColor.g = g;
		m_ClearColor.b = b;
		m_ClearColor.a = a;

		glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
	}
}