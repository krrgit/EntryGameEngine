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
		
		// We need to setup the light AFTER adding it to the light environment
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
			SetupLight();
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
		//LightLut_Phong(&m_Lut, shininess);
		//C3D_LightEnvLut(m_Parent, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_Lut);

		C3D_LightSpotEnable(&m_Light, false);
		C3D_LightDistAttnEnable(&m_Light, false);

		// Either all of them have DistAtten or none?
	}

	void Citro3DLight::SetAsPointLight(float shininess)
	{
		LightLutDA_Quadratic(&m_LutDA, 0.0f, 75.0f, 0.1f, 0.01f);
		C3D_LightDistAttn(&m_Light, &m_LutDA);

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

		C3D_LightDistAttnEnable(&m_Light, false);
	}

	void Citro3DLight::SetupLight()
	{
		switch (m_LightType)
		{
		case ET_DirectionalLight:
			SetAsDirectionalLight(m_Strength);
			break;
		case ET_PointLight:
			SetAsPointLight(m_Strength);
			break;
		case ET_Spotlight:
			SetAsSpotLight(m_Angle);
			break;
		default:
		break;
		}
	}
}