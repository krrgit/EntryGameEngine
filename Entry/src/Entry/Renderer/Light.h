#pragma once

#include "Entry/Core/Core.h"
#include <glm/glm.hpp>

namespace Entry
{
	enum LightType
	{
		Phong,
		Spotlight
	};

	struct LightProps
	{
		LightType Type;
		float Strength;
		float Angle;
		glm::vec3 Color;
		glm::vec3 InitPosition;
	};

	class Light 
	{
	public:
		virtual ~Light() = default;

		virtual void SetStrength(float strength) = 0;
		virtual void SetAngle(float angle) = 0;
		virtual void SetColor(glm::vec3 color) = 0;
		virtual void SetPosition(glm::vec3 position) = 0;

		static Ref<Light> Create(LightProps& props);
	};
}