#include "etpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
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

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		for (ECS::Entity e : m_Registry.view<TransformComponent, MeshRendererComponent>()) {
			TransformComponent& transform = m_Registry.get<TransformComponent>(e);
			MeshRendererComponent& meshRender= m_Registry.get<MeshRendererComponent>(e);

			Renderer3D::DrawMesh(meshRender.mesh, transform);
		}
	}
} 