#pragma once

#include "Entry/Window.h"
#include "Entry/LayerStack.h"

namespace Entry
{
	class Citro3DWindow : public Window
	{
	public:
		Citro3DWindow(const WindowProps& props);
		virtual ~Citro3DWindow();

		void OnUpdate(Timestep ts) override;
		void FrameDrawOn() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override {
			m_Data.EventCallback = callback; 
			hasEventCallback = true;
		}
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void PushLayer(Layer* layer) override;
		virtual void PushOverlay(Layer* layer) override;
		virtual void OnEvent(Event& e) override;

		inline virtual void* GetNativeWindow() const { return m_RenderTarget; }

		C3D_RenderTarget* const GetRenderTarget() { return m_RenderTarget; }

		void SetClearColor(uint32_t color) { m_ClearColor = color; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		void TriggerEvents();
	private:
		C3D_RenderTarget* m_RenderTarget;
		C3D_RenderTarget* m_RenderTargetR;
		LayerStack m_LayerStack;
		bool hasEventCallback;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			gfxScreen_t Screen;
			bool has2D;
			bool Stereo3D;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		
		uint32_t anyKeyPressed, anyKeyHeld, anyKeyReleased;
		bool readTouchPos;
		touchPosition touchPos;
		circlePosition circlePadPos;

		uint32_t m_ClearColor = 0x68B0D8FF;
	};
}