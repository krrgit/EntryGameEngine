#include "etpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entry/Renderer/Renderer3D.h"

#include <glm/glm.hpp>
#include <iostream>

namespace Entry {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	uint32_t Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdate(Timestep ts)
	{
		for (ECS::Entity e : m_Registry.view<TransformComponent, MeshRendererComponent>()) {
			TransformComponent* transform = m_Registry.get<TransformComponent>(e);
			MeshRendererComponent* meshRender= m_Registry.get<MeshRendererComponent>(e);

			Renderer3D::DrawMesh(meshRender->mesh, *transform);
		}
	}
}