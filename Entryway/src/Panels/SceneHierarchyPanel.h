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

		Entity GetSelectedEntity() const { return m_SelectionContext; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;

		Entity m_SelectionContext;
	};
}