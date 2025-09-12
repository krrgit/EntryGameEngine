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

		UpdateLight();
	}

	OpenGLLight::~OpenGLLight()
	{

	}

	void OpenGLLight::SetPosition(glm::vec3 position)
	{
	}

	void OpenGLLight::UpdateLight()
	{
		switch (m_LightType)
		{
		case LightType::Phong:
			break;
		case LightType::Spotlight:
			break;
		default:
		break;
		}
	}
}