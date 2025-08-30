#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Core/Log.h"
#include "Entry/Scene/Scene.h"
#include "Entry/Scene/Entity.h"

namespace Entry
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);
		
		void SetContext(const Ref<Scene>& context);
		
		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;

		Entity m_SelectionContext;
	};
}