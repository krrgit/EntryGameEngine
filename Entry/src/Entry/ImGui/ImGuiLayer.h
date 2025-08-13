#pragma once

#include "Entry/Core/Layer.h"
#include "Entry/Events/ButtonEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/CirclePadEvent.h"

#ifdef ET_PLATFORM_WINDOWS
#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/MouseEvent.h"
#include "Entry/Events/KeyEvent.h"
#endif // ET_PLATFORM_WINDOWS

namespace Entry {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;


		void Begin(Timestep ts);
		void End();

	private:
		bool OnScreenTouchedEvent(ScreenTouchedEvent& e);
		bool OnScreenReleasedEvent(ScreenReleasedEvent& e);
		bool OnButtonPressedEvent(ButtonPressedEvent& e);
		bool OnButtonReleasedEvent(ButtonReleasedEvent& e);
		bool OnCirclePadMovedEvent(CirclePadEvent& e);

#ifdef ET_PLATFORM_WINDOWS
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		
#endif // ET_PLATFORM_WINDOWS
	};
}