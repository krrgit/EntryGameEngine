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

	void Scene::OnUpdate(Timestep ts, uint16_t screenSide)
	{
		// Update Scripts 
		{
			m_Registry.view<NativeScriptComponent>().each([=](ECS::Entity entity, NativeScriptComponent& nsc) 
			{ 
				// TODO: Move to Scene::OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		// View: ideal for 1 component
		// Group: ideal for multiple components
		
		// Render Meshes
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view) 
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& camera = view.get<CameraComponent>(entity);

				if (camera.Primary) 
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera) 
		{
			Renderer3D::BeginScene(mainCamera->GetProjection(screenSide), *cameraTransform);

			for (ECS::Entity e : m_Registry.view<TransformComponent, MeshRendererComponent>()) {
				auto& transform = m_Registry.get<TransformComponent>(e);
				auto& meshRender= m_Registry.get<MeshRendererComponent>(e);

				Renderer3D::DrawMesh(meshRender.mesh, transform);
			}

			Renderer3D::EndScene();
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		//Resize non-FixedAspectRatio cameras
		for (auto entity : view) 
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}
} 