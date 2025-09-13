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

		static const C3D_Material material =
		{
			{ 0.2f, 0.2f, 0.2f }, //ambient
			{ 0.4f, 0.4f, 0.4f }, //diffuse
			{ 0.8f, 0.8f, 0.8f }, //specular0
			{ 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.0f, 0.0f, 0.0f }, //emission
		};

		C3D_LightEnvInit(&m_LightEnv);
		C3D_LightEnvBind(&m_LightEnv);
		C3D_LightEnvMaterial(&m_LightEnv, &material);

		LightLut_Phong(&m_Lut, m_Strength);
		C3D_LightEnvLut(&m_LightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_Lut);

		m_Position = FVec4_New(props.Position.x, props.Position.y, props.Position.z, 1.0f);

		C3D_LightInit(&m_Light, &m_LightEnv);
		C3D_LightColor(&m_Light, m_Color.r, m_Color.g, m_Color.b);
		C3D_LightPosition(&m_Light, &m_Position);
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