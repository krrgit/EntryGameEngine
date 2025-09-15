#include "etpch.h"
#include "OpenGLLight.h"

namespace Entry
{
	OpenGLLight::OpenGLLight(LightProps& props)
	{
		m_LightData = 
		{
			{0,0,0,0},
			{props.Color.r, props.Color.g, props.Color.b, 1.0f},
			{props.Strength, props.Angle, 0, 0},
			{0,0,0,0}
		};

		glGenBuffers(1, &m_LightUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOLightData), &m_LightData, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LightUBO); // binding = 0 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		m_MaterialData =
		{
			{ 0.2f, 0.2f, 0.2f, 0.0f }, //ambient
			{ 0.4f, 0.4f, 0.4f, 0.0f }, //diffuse
			{ 0.8f, 0.8f, 0.8f, 0.0f }, //specular0
			{ 0.0f, 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.0f, 0.0f, 0.0f, 0.0f }, //emission
		};

		glGenBuffers(1, &m_MaterialUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOMaterialData), &m_MaterialData, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_MaterialUBO); // binding = 1 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLLight::~OpenGLLight()
	{
		glDeleteBuffers(sizeof(UBOLightData), &m_LightUBO);
		glDeleteBuffers(sizeof(UBOMaterialData), &m_MaterialUBO);
	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_LightData = UBOLightData{};
		m_LightData.position = glm::vec4(props.Position, 1.0f);
		m_LightData.color = glm::vec4(props.Color, 1.0f);
		m_LightData.params.x = props.Strength;
		m_LightData.params.y = props.Angle;
		m_LightData.sceneAmbient = glm::vec4(0);

		glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOLightData), &m_LightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}