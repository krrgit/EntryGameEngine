#include "etpch.h"

#include "Entity.h"

namespace Entry 
{
	Entity::Entity(ECS::Entity handle, Scene* scene) 
		: m_EntityHandle(handle), m_Scene(scene)
	{

	}
}