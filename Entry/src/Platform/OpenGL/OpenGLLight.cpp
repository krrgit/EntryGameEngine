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

		switch (props.Type)
		{
		case ET_DirectionalLight:
		break;
		case ET_PointLight:
		break;
		case ET_Spotlight:
		break;
		default:
		break;
		}
	}

	OpenGLLight::~OpenGLLight()
	{
	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_LightData.position  = glm::vec4(props.Position, 1.0f);
		m_LightData.direction = glm::vec4(props.Direction, 1.0f);
		m_LightData.color     = glm::vec4(props.Color, 1.0f);
		m_LightData.params.x = (float)props.Type;
		m_LightData.params.y = props.Strength;
		m_LightData.params.z = props.Angle;


		glBindBuffer(GL_UNIFORM_BUFFER, m_Parent);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * m_LightID, sizeof(OGL_Light), &m_LightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLLight::SetAsDirectionalLight(float shininess)
	{
	}

	void OpenGLLight::SetAsPointLight(float shininess)
	{
	}

	void OpenGLLight::SetAsSpotLight(float angle)
	{
	}

	void OpenGLLight::SetupLight()
	{
	}
}