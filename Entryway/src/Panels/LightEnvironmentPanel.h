#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Scene/Scene.h"

namespace Entry
{
	class LightEnvironmentPanel
	{
	public:
		LightEnvironmentPanel() = default;
		LightEnvironmentPanel(const Ref<Scene>& context)
			: m_Context(context) 
		{ }

		~LightEnvironmentPanel() = default;

		void SetContext(const Ref<Scene>& context) { m_Context = context; }
		
		void OnImGuiRender();
	private:
		void RenderLut(ET_LIGHTLUTID lutID);
	
	private:
		Ref<Scene> m_Context;
	};
}