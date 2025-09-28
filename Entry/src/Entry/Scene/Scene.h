#pragma once

#include "Entry/Core/UUID.h"
#include "Entry/Core/Timestep.h"
#include "Entry/ECS/ecs.hpp"
#include "Entry/Renderer/EditorCamera.h"
#include "Entry/Renderer/LightEnvironment.h"

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

		Entity GetPrimaryCameraEntity();
		Entity GetLightEntity();

		int GetLightCount() { return m_LightCount; }
		bool LightLimitReached() { return m_LightCount >= MAX_LIGHTS; }

		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnv; }
		void SetLightEnvironment(Ref<LightEnvironment> lightEnv);

		void BindLightEnv();

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