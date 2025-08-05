#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Events/Event.h"
#include "Entry/Core/Timestep.h"

namespace Entry {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		/// <summary>
		/// Called when a layer is first added to the stack.
		/// </summary>
		virtual void OnAttach() {}
		/// <summary>
		/// Called when a layer is pushed off the layer stack.
		/// </summary>
		virtual void OnDetach() {}
		/// <summary>
		/// Called when ImGui is rendering.
		/// </summary>
		virtual void OnImGuiRender() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}