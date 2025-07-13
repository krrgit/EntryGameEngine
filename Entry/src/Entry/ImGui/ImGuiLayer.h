#pragma once

#include "Entry/Core/Layer.h"
#include "Entry/Events/KeyEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/CirclePadEvent.h"

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
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		//bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnCirclePadMovedEvent(CirclePadEvent& e);
	private:

	};
}