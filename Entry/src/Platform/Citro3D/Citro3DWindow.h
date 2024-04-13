#pragma once

#include "Entry/Window.h"

namespace Entry
{
	class Citro3DWindow : public Window
	{
	public:
		Citro3DWindow(const WindowProps& props);
		virtual ~Citro3DWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		C3D_RenderTarget* m_Window;
		C3D_RenderTarget* m_WindowR;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool Stereo3D;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}