#pragma once

#include "etpch.h"

#include "Entry/Core/Core.h"
#include "Entry/Events/Event.h"

namespace Entry {
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		uint16_t Screen; // 0 = TOP; 1 = BOTTOM
		bool Stereo3D;


		WindowProps(const std::string& title = "Entry Engine",
			unsigned int width = 400,
			unsigned int height = 240,
			uint16_t screen = 0,
			bool stereo3D = false)
			: Title(title), Width(width), Height(height), Screen(screen), Stereo3D(stereo3D)
		{
		}
	};

	// Interface representing a desktop(mobile has surface) system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate(Timestep ts) = 0;
		virtual void ScanHIDEvents() = 0;
		//virtual void FrameDrawOn() = 0;
		virtual void FrameEnd() = 0;


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