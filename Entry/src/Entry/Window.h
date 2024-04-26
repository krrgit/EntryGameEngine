#pragma once

#include "etpch.h"

#include "Entry/Core.h"
#include "Entry/Events/Event.h"

namespace Entry {
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		gfxScreen_t Screen;


		WindowProps(const std::string& title = "Entry Engine",
			unsigned int width = 400,
			unsigned int height = 240,
			gfxScreen_t screen = GFX_TOP)
			: Title(title), Width(width), Height(height), Screen(screen)
		{
		}
	};

	// Interface representing a desktop(mobile has surface) system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual void FrameDrawOn() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void PushLayer(Layer* layer) = 0;
		virtual void PushOverlay(Layer* layer) = 0;
		virtual void OnEvent(Event& e) = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}