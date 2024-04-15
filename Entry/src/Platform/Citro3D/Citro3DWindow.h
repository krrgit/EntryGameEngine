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
		void FrameBegin() override;
		void FrameEnd() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_RenderTarget; }

		C3D_RenderTarget* const GetRenderTarget() { return m_RenderTarget; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		void TriggerEvents();
	private:
		C3D_RenderTarget* m_RenderTarget;
		C3D_RenderTarget* m_RenderTargetR;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			gfxScreen_t Screen;
			bool Stereo3D;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		uint32_t anyKeyPressed, anyKeyHeld, anyKeyReleased;
		bool readTouchPos;
		touchPosition touchPos;
		circlePosition circlePadPos;
	};
}