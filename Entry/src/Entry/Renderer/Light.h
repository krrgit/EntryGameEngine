#pragma once

#include "Entry/Core/Core.h"
#include <glm/glm.hpp>

namespace Entry
{
	enum LightType
	{
		ET_DirectionalLight,
		ET_PointLight,
		ET_Spotlight
	};

	struct LightProps
	{
		glm::vec3 Position;
		glm::vec3 Direction;
		glm::vec3 Color;
		float Strength;
		float Angle;
		LightType Type;
	};

	class Light 
	{
	public:
		virtual ~Light() = default;

		virtual void SetLight(LightProps props) = 0;

		virtual void* GetNativeLight() = 0;

		virtual void SetID(int id) = 0;
		virtual int GetID() = 0;

		virtual void SetParent(uint32_t parent) = 0;

		virtual void SetAsDirectionalLight() = 0;
		virtual void SetAsPointLight(float linear, float quad) = 0;
		virtual void SetAsSpotLight(float angle) = 0;

		virtual void SetupLight() = 0;

		static Ref<Light> Create(LightProps& props);
	};
}