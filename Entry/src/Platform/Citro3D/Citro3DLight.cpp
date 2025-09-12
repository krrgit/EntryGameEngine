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

		m_Position = FVec4_New(props.InitPosition.x, props.InitPosition.y, props.InitPosition.z, 1.0f);

		C3D_LightInit(&m_Light, &m_LightEnv);
		C3D_LightColor(&m_Light, m_Color.r, m_Color.g, m_Color.b);
		C3D_LightPosition(&m_Light, &m_Position);
	}

	Citro3DLight::~Citro3DLight()
	{
	}

	void Citro3DLight::SetPosition(glm::vec3 position)
	{
		m_Position = FVec4_New(position.x, position.y, position.z, 1.0f);

		C3D_LightPosition(&m_Light, &m_Position);
	}

	void Citro3DLight::UpdateLight()
	{
		// TODO: Implement spotlight
		//switch (m_LightType)
		//{
		//case LightType::Phong:
		//	LightLut_Phong(&m_Lut, m_Strength);
		//	C3D_LightEnvLut(&m_LightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_Lut);
		//	break;
		//case LightType::Spotlight:
		//	LightLut_Spotlight(&m_Lut, m_Angle);
		//	C3D_LightEnvLut(&m_LightEnv, GPU_LUT_SP, GPU_LUTINPUT_LN, false, &m_Lut);
		//	break;
		//default:
		//break;
		//}
	}
}