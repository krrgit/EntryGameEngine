#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Core/Log.h"
#include "Entry/Scene/Scene.h"
#include "Entry/Scene/Entity.h"
#include "Entry/Scene/Components.h"

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
		void SetSelectedEntity(Entity entity);

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		void LoadMeshInMRC(std::string& filepath, std::string& currentPath, MeshRendererComponent& component);
	private:
		Ref<Scene> m_Context;

		Entity m_SelectionContext;
	};
}