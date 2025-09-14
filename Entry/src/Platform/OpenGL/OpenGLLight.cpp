#include "etpch.h"
#include "OpenGLLight.h"

namespace Entry
{
	OpenGLLight::OpenGLLight(LightProps& props)
	{
		m_LightType = props.Type;
		m_Strength = props.Strength;
		m_Angle = props.Angle;
		m_Color = props.Color;
		m_LightData = UBOLightData{};

		glGenBuffers(1, &m_LightUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOLightData), &m_LightData, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LightUBO); // binding = 0 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLLight::~OpenGLLight()
	{

	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_LightData = UBOLightData{};
		m_LightData.position = glm::vec4(props.Position, 1.0f);
		m_LightData.color = glm::vec4(props.Color, 1.0f);
		m_LightData.params.x = props.Strength;
		m_LightData.params.y = props.Angle;

		glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOLightData), &m_LightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}