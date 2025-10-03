#pragma once

#include "Entry/Core/UUID.h"
#include "Entry/Core/Timestep.h"
#include "Entry/Renderer/EditorCamera.h"
#include "Entry/Renderer/LightEnvironment.h"
#include "Entry/Renderer/Framebuffer.h"

#include "Entry/ECS/ecs.hpp"

#define MAX_LIGHTS 8

class q3Scene;

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

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep ts, uint16_t screenSide, EditorCamera& camera);
		void OnUpdateEditorInGame(Timestep ts, uint16_t screenSide);
		void OnUpdateRuntime(Timestep ts, uint16_t screenSide);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity(ET_GFX_SCREEN screen = ET_GFX_SCREEN::GFX_TOP);
		Entity GetLightEntity();

		int GetLightCount() { return m_LightCount; }
		bool LightLimitReached() { return m_LightCount >= MAX_LIGHTS; }

		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnv; }
		void BindLightEnv();

		template<typename... Components>
		ECS::Registry::View<Components...> GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:	
		void UpdateLights(glm::mat4& viewMatrix);

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		ECS::Registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		uint32_t m_LightCount = 0;
		Ref<LightEnvironment> m_LightEnv;

		q3Scene* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}