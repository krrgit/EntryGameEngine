#pragma once

#include "SceneCamera.h"
#include "Entry/Core/Core.h"
#include "Entry/Renderer/Light.h"
#include "Entry/Renderer/Model.h"
#include "Entry/Audio/AudioClip.h"
#include "Entry/Core/UUID.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Entry
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint64_t _id)
			: ID(_id)
		{}

	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };


		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position)
			: Position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position) 
				* rotation 
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct MeshRendererComponent
	{
		Ref<Model> model;
		const Mesh* mesh;
		Ref<Material> material;
		bool instancedMtl = false;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const Ref<Model> _model, const Mesh* _mesh, const Ref<Material> _material)
			: model(_model), mesh(_mesh), material(_material)
		{
		}

	};

	struct CameraComponent
	{
		Entry::SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to scene 
		bool FixedAspectRatio = true;
		ET_GFX_SCREEN RenderTarget;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// Forward Declaration
	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct LightComponent
	{
		LightType Type = LightType::ET_PointLight;
		float Intensity = 1.0f;
		float Range = 75.0f;
		float Angle = 90.0f;
		glm::vec3 Color = {1.0f, 1.0f, 1.0f};
		Ref<Light> RendererLight;

		LightComponent()
		{
			LightProps props{
				glm::vec3(0),
				glm::vec3(0,0,-1.0f),
				Color,
				Intensity,
				Range,
				Angle,
				Type,
			};

			RendererLight = Light::Create(props);
		}

		LightComponent(const LightComponent& other)
			: Type(other.Type), Intensity(other.Intensity), Range(other.Range), Angle(other.Angle), Color(other.Color)
		{
			LightProps props{
				glm::vec3(0.0f),
				glm::vec3(0,0,-1.0f),
				Color,
				Intensity,
				Range,
				Angle,
				Type,
			};

			RendererLight = Light::Create(props);
		}

		LightComponent(LightProps& props)
			: Type(props.Type), Intensity(props.Intensity), Range(props.Range), Angle(props.Angle), Color(props.Color)
		{
			RendererLight = Light::Create(props);
		}
	};

	// Physics 
	struct RigidbodyComponent
	{

		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		glm::bvec3 FixedRotation{ false, false, false };

		// Runtime storage
		void* RuntimeBody = nullptr;
		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};

	struct BoxColliderComponent
	{
		glm::vec3 Offset = { 0, 0, 0};
		glm::vec3 Size = { 0.5f, 0.5f, 0.5f };

		// TODO: maybe move into physics material?
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Runtime storage
		void* RuntimeFixture = nullptr;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct AudioSourceComponent
	{
		Ref<AudioClip> Clip;
		bool PlayOnAwake = true;

		AudioSourceComponent() = default;
		AudioSourceComponent(const AudioSourceComponent&) = default;

		void Play() const { Clip->Play(); }
		void Pause() const { Clip->Pause(); }
		void Stop() const { Clip->Stop(); }
	};
}