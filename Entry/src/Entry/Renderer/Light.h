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
		glm::vec3 Position;
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

		static Ref<Light> Create(LightProps& props);
	};
}