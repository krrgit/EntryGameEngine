#pragma once

#include "Entry/Core/Timestep.h"
#include "Entry/Renderer/EditorCamera.h"

#include "Entry/ECS/ecs.hpp"

namespace Entry {

	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts, uint16_t screenSide);
		void OnUpdateEditor(Timestep ts, uint16_t screenSide, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		ECS::Registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}