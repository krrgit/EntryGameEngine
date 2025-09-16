#include "etpch.h"
#include "Citro3DLight.h"

namespace Entry
{
	Citro3DLight::Citro3DLight(LightProps& props)
	{
		m_LightType = props.Type;
		m_Direction = props.Direction;
		m_Strength = props.Strength;
		m_Angle = props.Angle;
		m_Color = props.Color;

		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, 1.0f);

		switch (props.Type)
		{
		case ET_DirectionalLight:
			SetAsDirectionalLight(props.Strength);
			break;
		case ET_PointLight:
			SetAsPointLight(props.Strength);
			break;
		case ET_Spotlight:
			SetAsSpotLight(props.Angle);
			break;
		default:
			break;
		}
	}

	Citro3DLight::~Citro3DLight()
	{
	}

	void Citro3DLight::SetLight(LightProps props)
	{
		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, 1.0f);
		m_Color = props.Color;

		if (m_LightType != props.Type)
		{
			switch (props.Type)
			{
			case ET_DirectionalLight:
			SetAsDirectionalLight(props.Strength);
			break;
			case ET_PointLight:
			SetAsPointLight(props.Strength);
			break;
			case ET_Spotlight:
			SetAsSpotLight(props.Angle);
			break;
			default:
			break;
			}
		}

		C3D_LightPosition(&m_Light, &m_Position);
		C3D_LightColor(&m_Light, m_Color.r, m_Color.g, m_Color.b);

		if (m_LightType == ET_Spotlight)
		{
			C3D_LightSpotDir(&m_Light, m_Direction.x, m_Direction.y, m_Direction.z);
			C3D_LightSpotLut(&m_Light, &m_Lut);
		}
	}

	void Citro3DLight::SetAsDirectionalLight(float shininess)
	{
		LightLut_Phong(&m_Lut, shininess);
		C3D_LightEnvLut(m_Parent, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_Lut);
	}

	void Citro3DLight::SetAsPointLight(float shininess)
	{
		LightLut_Phong(&m_Lut, shininess);
		C3D_LightEnvLut(m_Parent, GPU_LUT_DA, GPU_LUTINPUT_LN, false, &m_Lut);
		C3D_LightSpotEnable(&m_Light, false);
	}

	void Citro3DLight::SetAsSpotLight(float angle)
	{
		float realAngle = angle / 90.0f;
		LightLut_Spotlight(&m_Lut, realAngle);
		C3D_LightEnvLut(m_Parent, GPU_LUT_SP, GPU_LUTINPUT_LN, true, &m_Lut);

		C3D_LightSpotEnable(&m_Light, true);
		C3D_LightSpotDir(&m_Light, m_Direction.x, m_Direction.y, m_Direction.z);
		C3D_LightSpotLut(&m_Light, &m_Lut);
	}

	void Citro3DLight::CreateQuadraticLut(float from, float to, float linear, float quad)
	{
		LightLutDA_Quadratic(&m_LutDA, from, to, linear, quad);
	}
}