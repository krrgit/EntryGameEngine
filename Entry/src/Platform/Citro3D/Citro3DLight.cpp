#include "etpch.h"
#include "Citro3DLight.h"

namespace Entry
{
	Citro3DLight::Citro3DLight(LightProps& props)
	{
		m_LightType = props.Type;
		m_Strength = props.Strength;
		m_Angle = props.Angle;
		m_Color = props.Color;

		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, 1.0f);
	}

	Citro3DLight::~Citro3DLight()
	{
	}

	void Citro3DLight::SetLight(LightProps props)
	{
		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, 1.0f);
		m_Color = props.Color;

		C3D_LightPosition(&m_Light, &m_Position);
		C3D_LightColor(&m_Light, m_Color.r, m_Color.g, m_Color.b);
	}

}