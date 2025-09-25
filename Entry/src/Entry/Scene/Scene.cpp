#include "etpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Entity.h"
#include "Entry/Renderer/Renderer3D.h"

#include <glm/glm.hpp>
#include <iostream>


namespace Entry {

	Scene::Scene()
	{
		m_LightEnv = LightEnvironment::Create();
		m_LightEnv->Bind();

		m_LightEnv->SetSceneAmbientColor(glm::vec3(0));

		MaterialProps matProps;

		matProps.Values = {
				{ 0.2f, 0.2f, 0.2f }, //ambient
				{ 0.8f, 0.8f, 0.8f }, //diffuse
				{ 1.0f, 1.0f, 1.0f }, //specular0
				{ 0.0f, 0.0f, 0.0f }, //specular1
				{ 0.0f, 0.0f, 0.0f }, //emission
		};

		Ref<Material> sampleMat = Material::Create(matProps);
		m_LightEnv->BindMaterial(sampleMat.get());

		//auto& lutconfig = GetLightEnvironment()->GetLutConfig(ET_LIGHTLUTID::ET_LUT_D0);
		//lutconfig.input = ET_LIGHTLUTINPUT::ET_LUTINPUT_NH;
		//lutconfig.funcType = LutFuncType::Pow;
		//lutconfig.funcArgs.powExponent = 1000.0f;
		//GetLightEnvironment()->ConfigureLut(lutconfig);

		m_Registry.on_construct<LightComponent>().connect([&](ECS::Entity e, LightComponent& l) {
			ET_CORE_INFO("Lights: {0}", ++m_LightCount);
			m_LightEnv->LightInit(l.RendererLight);
			if (m_LightCount == MAX_LIGHTS)
				ET_CORE_WARN("Maximum Light Limit (8) for 3DS reached.");
		});

		m_Registry.on_destroy<LightComponent>().connect([&](ECS::Entity e, LightComponent& l) {
			m_LightEnv->LightDestroy(l.RendererLight);
			ET_CORE_INFO("Lights: {0}", --m_LightCount);
		});

		Renderer3D::BindLightEnv(m_LightEnv);
	}

	Scene::~Scene()
	{
		ET_CORE_INFO("Destroy Scene");
	}

	template<typename Component>
	static void CopyComponent(ECS::Registry& dst, ECS::Registry& src, const std::unordered_map<UUID, ECS::Entity>& entityMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			ET_CORE_ASSERT(entityMap.find(uuid) != entityMap.end());
			ECS::Entity dstEntityID = entityMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEntityID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene;
		newScene.reset(new Scene());

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, ECS::Entity> entityMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;

		// Create Entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto& e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			entityMap[uuid] = (ECS::Entity)newEntity;
		}

		// Copy Components (except IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<MeshRendererComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, uint16_t screenSide, EditorCamera& camera)
	{
		Renderer3D::BeginScene(camera, screenSide);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		UpdateLights(viewMatrix);

		for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
		{
			auto transform = m_Registry.get<TransformComponent>(entity);
			auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

			if (!meshRenderer.model) continue;
			Renderer3D::DrawMeshEntity(meshRenderer, transform.GetTransform(), entity);
		}

		Renderer3D::EndScene();
	}

	/// <summary>
	/// Renders the camera in-game when in editor mode.
	/// </summary>
	/// <param name="ts"></param>
	/// <param name="screenSide"></param>
	void Scene::OnUpdateEditorInGame(Timestep ts, uint16_t screenSide)
	{
		// Render Modeles
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::mat4 viewMatrix;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& camera = view.get<CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					viewMatrix = glm::inverse(cameraTransform);
					break;
				}
			}
		}

		UpdateLights(viewMatrix);

		if (mainCamera)
		{
			Renderer3D::BeginScene(mainCamera->GetProjection(screenSide), cameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
			{
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) continue;
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
			}

			Renderer3D::EndScene();
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts, uint16_t screenSide)
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
		glm::mat4 viewMatrix(1.0f);
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view) 
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& camera = view.get<CameraComponent>(entity);

				if (camera.Primary) 
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					viewMatrix = glm::inverse(transform.GetTransform());
					break;
				}
			}
		}

		// Update Lights
		UpdateLights(viewMatrix);

		if (mainCamera) 
		{
			Renderer3D::BeginScene(mainCamera->GetProjection(screenSide), cameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>()) {
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) {
					continue; 
				}
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
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
			else 
				cameraComponent.Camera.SetViewportSize(width, height); // Do something else?
		}
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<MeshRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		if (!LightLimitReached())
		{
			CopyComponentIfExists<LightComponent>(newEntity, entity);
		}
		else
		{
			ET_CORE_WARN("Light Limit Reached: Cannot duplicate light.");
		}
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);

		return newEntity;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity { entity, this };
		}

		return {};
	}

	Entity Scene::GetLightEntity()
	{
		auto view = m_Registry.view<LightComponent>();
		for (auto entity : view)
		{
			return Entity{ entity, this };
		}
	}

	void Scene::UpdateLights(glm::mat4& viewMatrix)
	{
		auto view = m_Registry.view<TransformComponent, LightComponent>();
		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);
			auto& light = view.get<LightComponent>(entity);

			auto clip = viewMatrix * glm::vec4(transform.Position, 1.0f);
			auto viewTransform = viewMatrix * transform.GetTransform();
			glm::vec3 clipPos{ clip.x, clip.y, clip.z };
			light.RendererLight->SetLight({ viewTransform[3], viewTransform[2], light.Color, light.Strength, light.Angle, light.Type});
		}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{

		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{

	}
} 