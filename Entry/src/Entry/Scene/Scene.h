#pragma once

#include "Entry/Core/Timestep.h"

#include "Entry/ECS/ecs.hpp"

namespace Entry {

	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts, uint16_t screenSide);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		ECS::Registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}