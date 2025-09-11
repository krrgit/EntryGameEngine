#pragma once

#include "Scene.h"
#include "Entry/Core/UUID.h"
#include "Entry/ECS/ecs.hpp"
#include "Components.h"


namespace Entry {

	class Entity {
	public:
		Entity() = default;
		Entity(ECS::Entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			ET_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T>
		T& GetComponent() {
			ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent() {
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != 0; }
		//operator ECS::Entity() const { return m_EntityHandle; } // operator uint32_t() is the same
		operator uint32_t() const { return m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		bool operator==(const Entity& other) const { 
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	private:
		ECS::Entity m_EntityHandle { 0 };
		Scene* m_Scene = nullptr;
	};
}