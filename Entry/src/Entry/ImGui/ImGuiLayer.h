#pragma once

#include "Entry/Layer.h"
#include "Entry/Events/KeyEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/CirclePadEvent.h"

namespace Entry {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		bool OnScreenTouchedEvent(ScreenTouchedEvent& e);
		bool OnScreenReleasedEvent(ScreenReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		//bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnCirclePadMovedEvent(CirclePadEvent& e);
	private:
		time_t m_Time;
	};
}