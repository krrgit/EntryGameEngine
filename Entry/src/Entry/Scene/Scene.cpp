#include "etpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Entity.h"
#include "Entry/Renderer/Renderer3D.h"
#include "Entry/Renderer/RenderCommand.h"

#include <glm/glm.hpp>
#include <iostream>

#include <q3.h>

namespace Entry {

	static q3BodyType RigidbodyTypeToQu3eBody(RigidbodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case RigidbodyComponent::BodyType::Static:		return q3BodyType::eStaticBody;
			case RigidbodyComponent::BodyType::Dynamic:		return q3BodyType::eDynamicBody;
			case RigidbodyComponent::BodyType::Kinematic:	return q3BodyType::eKinematicBody;
		}
		ET_CORE_ASSERT(false, "Unknown body type");
		return q3BodyType::eStaticBody;
	}

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

		newScene->GetLightEnvironment()->SetSceneAmbientColor(other->GetLightEnvironment()->GetSceneAmbientColor());

		ET_LIGHTLUTID ids[] = { ET_LUT_D0, ET_LUT_D1, ET_LUT_FR, ET_LUT_RB, ET_LUT_RG, ET_LUT_RR };

		for (int i = 0; i < 6; i++)
		{
			auto& lutconfig = other->GetLightEnvironment()->GetLutConfig(ids[i]);
			newScene->GetLightEnvironment()->ConfigureLut(lutconfig);
		}

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
		CopyComponent<LightComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<RigidbodyComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);
		CopyComponent<BoxColliderComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);

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

	void Scene::OnRuntimeStart()
	{
		// Create Physics Simulation
		m_PhysicsWorld = new q3Scene(1.0f / 60.0f);

		auto view = m_Registry.view<RigidbodyComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb = entity.GetComponent<RigidbodyComponent>();

			q3BodyDef bodyDef;
			bodyDef.bodyType = RigidbodyTypeToQu3eBody(rb.Type);
			bodyDef.position = { transform.Position.x, transform.Position.y, transform.Position.z };

			glm::quat quat = glm::quat(transform.Rotation);
			bodyDef.rotation = q3Quaternion(quat.x, quat.y, quat.z, quat.w);

			bodyDef.angle = transform.Rotation.z;
			bodyDef.lockAxisX = rb.FixedRotation.x;
			bodyDef.lockAxisY = rb.FixedRotation.y;
			bodyDef.lockAxisZ = rb.FixedRotation.z;

			q3Body* body = m_PhysicsWorld->CreateBody(bodyDef);
			rb.RuntimeBody = body;

			if (entity.HasComponent<BoxColliderComponent>()) 
			{
				auto& bc = entity.GetComponent<BoxColliderComponent>();

				q3BoxDef boxDef;
				q3Transform localSpace;
				q3Identity(localSpace);

				boxDef.SetDensity(bc.Density);
				boxDef.SetFriction(bc.Friction);
				boxDef.SetRestitution(bc.Restitution);
				q3Vec3 boxSize = q3Vec3(bc.Size.x * transform.Scale.x, bc.Size.y * transform.Scale.y, bc.Size.z * transform.Scale.z);
				localSpace.position.x = bc.Offset.x;
				localSpace.position.y = bc.Offset.y;
				localSpace.position.z = bc.Offset.z;

				boxDef.Set(localSpace, boxSize);
				body->AddBox(boxDef);
			}
		}

		// Create Native Scripts
		m_Registry.view<NativeScriptComponent>().each([=](ECS::Entity entity, NativeScriptComponent& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}
		});
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
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
		// Render Meshes
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::mat4 viewMatrix;

		SceneCamera* bottomCamera = nullptr;
		glm::mat4 bottomCameraTransform;
		glm::mat4 bottomViewMatrix;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& camera = view.get<CameraComponent>(entity);

				if (camera.Primary)
				{
					if (camera.RenderTarget == ET_GFX_SCREEN::GFX_TOP)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform.GetTransform();
						viewMatrix = glm::inverse(cameraTransform);
						//break;
					}
					else if (camera.RenderTarget == ET_GFX_SCREEN::GFX_BOTTOM)
					{
						bottomCamera = &camera.Camera;
						bottomCameraTransform = transform.GetTransform();
						bottomViewMatrix = glm::inverse(bottomCameraTransform);
					}
				}
			}
		}


		if (mainCamera)
		{
			UpdateLights(viewMatrix);

			mainCamera->GetFramebuffer()->Bind();
			RenderCommand::SetClearColor(0x68B0D8FF);
			RenderCommand::Clear();
			
			Renderer3D::BeginScene(mainCamera->GetProjection(screenSide), cameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
			{
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) continue;
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
			}

			Renderer3D::EndScene();
			mainCamera->GetFramebuffer()->Unbind();
		}

		if (bottomCamera)
		{
			UpdateLights(bottomViewMatrix);

			bottomCamera->GetFramebuffer()->Bind();
			RenderCommand::SetClearColor(0x68B0D8FF);
			RenderCommand::Clear();

			Renderer3D::BeginScene(bottomCamera->GetProjection(screenSide), bottomCameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
			{
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) continue;
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
			}

			Renderer3D::EndScene();
			bottomCamera->GetFramebuffer()->Unbind();
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts, uint16_t screenSide)
	{
		// Update Scripts 
		{
			m_Registry.view<NativeScriptComponent>().each([=](ECS::Entity entity, NativeScriptComponent& nsc) 
			{ 
				nsc.Instance->OnUpdate(ts);
			});
		}

		// Physics
		{
			m_PhysicsWorld->Step();
			
			// Retrieve transform form qu3e
			auto view = m_Registry.view<RigidbodyComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<RigidbodyComponent>();

				q3Body* body = (q3Body*)rb.RuntimeBody;
				const auto& q3transform = body->GetTransform();
				transform.Position.x = q3transform.position.x;
				transform.Position.y = q3transform.position.y;
				transform.Position.z = q3transform.position.z;

				// Set Rotation
				// Assuming right-handed system, Y-up
				const auto& r = body->GetTransform().rotation;
				float sy = -r.ex.z;
				float cy = sqrtf(r.ex.x * r.ex.x + r.ex.y * r.ex.y);

				float pitch, yaw, roll;

				if (cy > 1e-6f)
				{
					pitch = atan2f(r.ey.z, r.ez.z);
					yaw = atan2f(sy, cy);
					roll = atan2f(r.ex.y, r.ex.x);
				}
				else
				{
					// Gimbal lock fallback
					pitch = atan2f(-r.ez.y, r.ey.y);
					yaw = atan2f(sy, cy);
					roll = 0.0f;
				}

				transform.Rotation.x = pitch; // or however your engine orders XYZ
				transform.Rotation.y = yaw;
				transform.Rotation.z = roll;
			}
		}

		// View: ideal for 1 component
		// Group: ideal for multiple components
		
		// Render Meshes
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::mat4 viewMatrix;

		SceneCamera* touchCamera = nullptr;
		glm::mat4 touchCameraTransform;
		glm::mat4 touchViewMatrix;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& camera = view.get<CameraComponent>(entity);

				if (camera.Primary && camera.RenderTarget == ET_GFX_SCREEN::GFX_TOP)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					viewMatrix = glm::inverse(cameraTransform);
					//break;
				}
				if (camera.Primary && camera.RenderTarget == ET_GFX_SCREEN::GFX_BOTTOM)
				{
					touchCamera = &camera.Camera;
					touchCameraTransform = transform.GetTransform();
					touchViewMatrix = glm::inverse(touchCameraTransform);
					//break;
				}
			}
		}


		if (mainCamera)
		{
			UpdateLights(viewMatrix);

			mainCamera->GetFramebuffer()->Bind();
			RenderCommand::SetClearColor(0x68B0D8FF);
			RenderCommand::Clear();

			Renderer3D::BeginScene(mainCamera->GetProjection(screenSide), cameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
			{
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) continue;
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
			}

			Renderer3D::EndScene();
			mainCamera->GetFramebuffer()->Unbind();
		}

		if (touchCamera)
		{
			UpdateLights(touchViewMatrix);

			touchCamera->GetFramebuffer()->Bind();
			RenderCommand::SetClearColor(0x68B0D8FF);
			RenderCommand::Clear();

			Renderer3D::BeginScene(touchCamera->GetProjection(screenSide), touchCameraTransform);

			for (ECS::Entity entity : m_Registry.view<TransformComponent, MeshRendererComponent>())
			{
				auto transform = m_Registry.get<TransformComponent>(entity);
				auto& meshRenderer = m_Registry.get<MeshRendererComponent>(entity);

				if (!meshRenderer.model) continue;
				Renderer3D::DrawMesh(meshRenderer, transform.GetTransform());
			}

			Renderer3D::EndScene();
			touchCamera->GetFramebuffer()->Unbind();
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
		CopyComponentIfExists<LightComponent>(newEntity, entity);
		CopyComponentIfExists<RigidbodyComponent>(newEntity, entity);
		CopyComponentIfExists<BoxColliderComponent>(newEntity, entity);

		return newEntity;
	}

	Entity Scene::GetPrimaryCameraEntity(ET_GFX_SCREEN screen)
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary && camera.RenderTarget == screen)
			{
				return Entity { entity, this };
			}
		}

		return Entity();
	}

	Entity Scene::GetLightEntity()
	{
		auto view = m_Registry.view<LightComponent>();
		for (auto entity : view)
		{
			return Entity{ entity, this };
		}

		return Entity();
	}

	void Scene::BindLightEnv()
	{
		m_LightEnv->Bind();
		Renderer3D::BindLightEnv(m_LightEnv);
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
			light.RendererLight->SetLight({ viewTransform[3], viewTransform[2], light.Color, light.Intensity, light.Range, light.Angle, light.Type});
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

	template<>
	void Scene::OnComponentAdded<RigidbodyComponent>(Entity entity, RigidbodyComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component)
	{

	}
} 