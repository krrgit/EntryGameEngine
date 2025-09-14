#pragma once

#include "Entry/Renderer/Light.h"

#include <glad/glad.h>

namespace Entry
{
	struct UBOLightData
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 params;
	};

	class OpenGLLight : public Light
	{
	public:
		OpenGLLight(LightProps& props);
		virtual ~OpenGLLight();

		virtual void SetLight(LightProps props) override;
	private:
		GLuint m_LightUBO;
		UBOLightData m_LightData;

		LightType m_LightType;
		float m_Strength;
		float m_Angle;
		glm::vec3 m_Color;
	};
}