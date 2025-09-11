#pragma once

#include "Entry/Core/UUID.h"
#include "Entry/Core/Timestep.h"
#include "Entry/ECS/ecs.hpp"
#include "Entry/Renderer/EditorCamera.h"

namespace Entry {

	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateEditor(Timestep ts, uint16_t screenSide, EditorCamera& camera);
		void OnUpdateEditorInGame(Timestep ts, uint16_t screenSide);
		void OnUpdateRuntime(Timestep ts, uint16_t screenSide);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

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