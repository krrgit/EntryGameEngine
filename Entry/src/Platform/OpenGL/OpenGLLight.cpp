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
	}

	OpenGLLight::~OpenGLLight()
	{

	}

	void OpenGLLight::SetLight(LightProps props)
	{
	}
}