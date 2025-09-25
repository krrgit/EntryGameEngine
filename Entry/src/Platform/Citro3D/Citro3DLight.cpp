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
		m_PositionalLight = props.Type == ET_DirectionalLight ? 0.0f : 1.0f;

		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, m_PositionalLight);
		
		// We need to setup the light AFTER adding it to the light environment
	}

	Citro3DLight::~Citro3DLight()
	{
	}

	void Citro3DLight::SetLight(LightProps props)
	{
		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, m_PositionalLight);
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

	void Citro3DLight::SetAsDirectionalLight()
	{
		C3D_LightSpotEnable(&m_Light, false);
		C3D_LightDistAttnEnable(&m_Light, false);
	
		// Old Code: no attenuation
		//LightLut_Phong(&m_Lut, shininess);
		//C3D_LightEnvLut(m_Parent, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_Lut);
	}

	void Citro3DLight::SetAsPointLight(float linear, float quad)
	{
		// TODO: Setup customization for quadratic in LightEnv
		LightLutDA_Quadratic(&m_LutDA, 0.0f, 75.0f, linear, quad);
		C3D_LightDistAttn(&m_Light, &m_LutDA);

		C3D_LightSpotEnable(&m_Light, false);
	}

	void Citro3DLight::SetAsSpotLight(float angle)
	{
		float realAngle = angle / 90.0f;
		LightLut_Spotlight(&m_Lut, realAngle);
		C3D_LightEnvLut(m_Parent, GPU_LUT_SP, GPU_LUTINPUT_LN, true, &m_Lut);
		
		C3D_LightSpotDir(&m_Light, m_Direction.x, m_Direction.y, m_Direction.z);
		C3D_LightSpotLut(&m_Light, &m_Lut);
		
		C3D_LightDistAttnEnable(&m_Light, false);
	}

	void Citro3DLight::SetupLight()
	{
		switch (m_LightType)
		{
		case ET_DirectionalLight:
			SetAsDirectionalLight();
			m_PositionalLight = 0.0f;
			break;
		case ET_PointLight:
			SetAsPointLight(0.1f, 0.01f);
			m_PositionalLight = 1.0f;
			break;
		case ET_Spotlight:
			SetAsSpotLight(m_Angle);
			m_PositionalLight = 1.0f;
			break;
		default:
		break;
		}
	}
}