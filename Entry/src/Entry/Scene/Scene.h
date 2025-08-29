#pragma once

#include "Entry/Core/Timestep.h"

#include "Entry/ECS/ecs.hpp"

namespace Entry {
	class Scene 
	{
	public:
		Scene();
		~Scene();

		uint32_t CreateEntity();

		ECS::Registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep ts);
	private:
		ECS::Registry m_Registry;
	};
}