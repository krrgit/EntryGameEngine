#include "etpch.h"
#include "OpenGLLight.h"

namespace Entry
{

	OpenGLLight::OpenGLLight(LightProps& props)
	{ 
		m_LightData.position = glm::vec4(props.Position, 1.0f);
		m_LightData.color = glm::vec4(props.Color, 1.0f);
		m_LightData.params.x = 1.0f;
		m_LightData.params.y = props.Strength;
		m_LightData.params.z = props.Angle;
	}

	OpenGLLight::~OpenGLLight()
	{
	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_LightData.position = glm::vec4(props.Position, 1.0f);
		m_LightData.color = glm::vec4(props.Color, 1.0f);
		m_LightData.params.x = 1.0f;
		m_LightData.params.y = props.Strength;
		m_LightData.params.z = props.Angle;

		glBindBuffer(GL_UNIFORM_BUFFER, m_Parent);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * m_LightID, sizeof(OGL_Light), &m_LightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}