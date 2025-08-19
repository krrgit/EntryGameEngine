#include "etpch.h"
#include "WindowsWindow.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/KeyEvent.h"
#include "Entry/Events/MouseEvent.h"
#include <Entry/Core/KeyCodes.h>
//#include "Entry/Events/CirclePadEvent.h"
//#include "Entry/Events/Slider3DEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Entry
{
	static uint8_t s_GLFWWindowCount = 0;

	GLFWwindow* s_FirstWindow = nullptr;

	static void GLFWErrorCallback(int error, const char* description) 
	{
		ET_CORE_ERROR("GLFW Error ({0}: {1}", error, description);
	}

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
		m_Data.Screen = s_GLFWWindowCount;

		ET_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		

		if (m_Data.Screen == 0)
		{
			ET_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
			ET_CORE_ASSERT(success, "Could not initialize GLFW!");
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, s_FirstWindow);
		++s_GLFWWindowCount;

		if (s_FirstWindow == nullptr) {
			s_FirstWindow = m_Window;
			m_Context = new OpenGLContext(m_Window);
			m_Context->Init();
		}
		
		if (m_Data.Screen == 0) glfwSetWindowAspectRatio(m_Window, props.Width, props.Height); // Keeps aspect ratio
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
		
		if (m_Data.Screen >= 0) {

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height, (void*)window);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event((KeyCode)key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event((KeyCode)key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event((KeyCode)keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	
		}
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
		if (m_Data.Screen == 0) glfwPollEvents();

		glfwMakeContextCurrent(m_Window);
		glfwSwapBuffers(m_Window);
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
		m_LayerStack.PushOverlay(layer);
	}

	void WindowsWindow::OnEvent(Event& e)
	{
	}

	//void WindowsWindow::SetClearColor(uint32_t color)
	//{

	//}
}