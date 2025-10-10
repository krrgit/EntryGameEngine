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

		~SceneHierarchyPanel() = default;

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

		void DeleteSelectedEntity();

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		void LoadMeshInMRC(std::string& filepath, int meshID, MeshRendererComponent& component);

		void LoadAudioClipInASC(std::string& filepath, AudioSourceComponent& component);
	private:
		Ref<Scene> m_Context;

		Entity m_SelectionContext;

		Entity m_EntityToDuplicate;
	};
}