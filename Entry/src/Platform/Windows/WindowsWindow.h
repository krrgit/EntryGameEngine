#pragma once

#include "Entry/Core/Window.h"
#include "Entry/Core/LayerStack.h"
#include "Platform/Windows/GraphicsContext.h"

#include "glm/glm.hpp"

#include <GLFW/glfw3.h>

namespace Entry
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate(Timestep ts) override;
		void ScanHIDEvents() override;
		void FrameEnd() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override
		{
			m_Data.EventCallback = callback;
			hasEventCallback = true;
		}
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void PushLayer(Layer* layer) override;
		virtual void PushOverlay(Layer* layer) override;
		virtual void OnEvent(Event& e) override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

		//void SetClearColor(uint32_t color);
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		
		void LayerStackOnUpdate(Timestep ts, uint16_t screenSide = 0);
		void FrameDrawOn();

	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;
		LayerStack m_LayerStack;
		bool hasEventCallback;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			uint32_t Screen;
			bool has2D;
			bool Stereo3D;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		glm::vec4 m_ClearColor;
	};

}
