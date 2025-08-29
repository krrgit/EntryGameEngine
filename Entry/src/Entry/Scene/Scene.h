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
	private:
		ECS::Registry m_Registry;

		friend class Entity;
	};
}